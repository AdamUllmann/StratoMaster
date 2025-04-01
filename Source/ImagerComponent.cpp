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

    // Stereoize (unimplemented)
    /*stereoizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    stereoizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    addAndMakeVisible(stereoizeSlider);

    attachments.stereoizeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "ImagerStereoize", stereoizeSlider));

    stereoizeLabel.setText("Stereoize", juce::dontSendNotification);
    stereoizeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(stereoizeLabel); */
    startTimerHz(120);
}

ImagerComponent::~ImagerComponent()
{
    stopTimer();
}

void ImagerComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto leftArea = area.removeFromLeft(area.getWidth() / 2);
    auto labelHeight = 20;
    widthLabel.setBounds(leftArea.removeFromTop(labelHeight));
    widthSlider.setBounds(leftArea.withSizeKeepingCentre(300, 300));
    auto rightArea = area;
    int side = juce::jmin(rightArea.getWidth(), rightArea.getHeight());
    scopeArea = juce::Rectangle<int>(rightArea.getCentreX() - side / 2, rightArea.getCentreY() - side / 2, side, side);
}

void ImagerComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(30, 30, 30), 0, 0, juce::Colour(30, 30, 30), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();
    g.setColour(juce::Colours::grey);
    g.drawRect(scopeArea);
    const int cx = scopeArea.getCentreX();
    const int cy = scopeArea.getCentreY();
    g.drawLine((float)scopeArea.getX(), (float)cy, (float)scopeArea.getRight(), (float)cy, 1.0f);
    g.drawLine((float)cx, (float)scopeArea.getY(), (float)cx, (float)scopeArea.getBottom(), 1.0f);
    const int textOffset = 20;
    g.drawFittedText("L", scopeArea.getX() + 4, cy - textOffset, 20, 20, juce::Justification::left, 1);
    g.drawFittedText("R", scopeArea.getRight() - 25, cy - textOffset, 20, 20, juce::Justification::right, 1);
    g.drawFittedText("+1", cx, scopeArea.getY() + 2, 20, 20, juce::Justification::centred, 1);
    g.drawFittedText("-1", cx, scopeArea.getBottom() - 20, 20, 20, juce::Justification::centred, 1);
    g.setColour(juce::Colours::cyan.withAlpha(0.8f));
    const int scopeIndex = audioProcessor.getScopeIndex();
    const int scopeSize = audioProcessor.getScopeSize();
    const float* bufferData = audioProcessor.getScopeBuffer();
    float radius = (float)juce::jmin(scopeArea.getWidth(), scopeArea.getHeight()) / 2 - 5;
    for (int i = 0; i < scopeSize; ++i) {
        int idx = (scopeIndex + i) % scopeSize;
        float L = bufferData[idx * 2 + 0];
        float R = bufferData[idx * 2 + 1];
        float X = (L - R) * 0.7071f; // square root of 2
        float Y = (L + R) * 0.7071f;
        float drawX = cx - X * radius;
        float drawY = cy - Y * radius;
        g.fillEllipse(drawX - 1, drawY - 1, 2, 2);
    }
}