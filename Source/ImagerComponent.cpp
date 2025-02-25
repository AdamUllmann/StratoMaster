/*
  ==============================================================================

    ImagerComponent.cpp
    Created: 24 Feb 2025 2:02:44pm
    Author:  Adam

  ==============================================================================
*/

#include "ImagerComponent.h"


ImagerComponent::ImagerComponent(StratomasterAudioProcessor& proc, juce::AudioProcessorValueTreeState& apvts) : audioProcessor(proc), apvtsRef(apvts)
{
    // Width
    widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    addAndMakeVisible(widthSlider);

    attachments.widthAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "ImagerWidth", widthSlider));

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widthLabel);

    // Stereoize
    stereoizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    stereoizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    addAndMakeVisible(stereoizeSlider);

    attachments.stereoizeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "ImagerStereoize", stereoizeSlider));

    stereoizeLabel.setText("Stereoize", juce::dontSendNotification);
    stereoizeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(stereoizeLabel);
    startTimerHz(60);
}

ImagerComponent::~ImagerComponent()
{
    stopTimer();
}

void ImagerComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto topHeight = 120;
    auto topArea = area.removeFromTop(topHeight);
    widthLabel.setBounds(topArea.removeFromTop(20));
    widthSlider.setBounds(topArea.withSizeKeepingCentre(60, 60));
    auto stereoHeight = 50;
    auto stereoArea = area.removeFromTop(stereoHeight);
    stereoizeLabel.setBounds(stereoArea.removeFromLeft(70));
    stereoizeSlider.setBounds(stereoArea.reduced(5));
    scopeArea = area;
}

void ImagerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(35, 35, 35));
    g.setColour(juce::Colours::grey);
    g.drawRect(scopeArea);
    int scopeIndex = audioProcessor.getScopeIndex();
    int scopeSize = audioProcessor.getScopeSize();
    const float* bufferData = audioProcessor.getScopeBuffer();
    auto cx = scopeArea.getCentreX();
    auto cy = scopeArea.getCentreY();
    auto radius = (float)juce::jmin(scopeArea.getWidth(), scopeArea.getHeight()) / 2 - 5;
    g.setColour(juce::Colours::cyan.withAlpha(0.8f));
    for (int i = 0; i < scopeSize; ++i)
    {
        int index = (scopeIndex + i) % scopeSize;
        float L = bufferData[index * 2 + 0];
        float R = bufferData[index * 2 + 1];
        float X = (L + R) * 0.7071f;  // x = L, y = R for a standard lissajous
        float Y = (L - R) * 0.7071f;
        float drawX = cx + X * radius;
        float drawY = cy - Y * radius;  // subtract Y to invert vertical

        g.fillEllipse(drawX - 1, drawY - 1, 2.0f, 2.0f);
    }
}