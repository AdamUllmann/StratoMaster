/*
  ==============================================================================

    CompressorComponent.cpp
    Created: 3 Feb 2025 4:05:11am
    Author:  Adam

  ==============================================================================
*/

#include "CompressorComponent.h"

namespace
{
    const juce::Colour thresholdColour = juce::Colour::fromRGB(200, 70, 70);
    const juce::Colour ratioColour = juce::Colour::fromRGB(70, 200, 70);
    const juce::Colour attackColour = juce::Colour::fromRGB(70, 70, 200);
    const juce::Colour releaseColour = juce::Colour::fromRGB(200, 200, 70);
    const juce::Colour makeupColour = juce::Colour::fromRGB(70, 200, 200);
}

//==============================================================================
CompressorComponent::CompressorComponent(juce::AudioProcessorValueTreeState& apvts) : apvtsRef(apvts)
{
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 20);
    thresholdSlider.setColour(juce::Slider::rotarySliderFillColourId, thresholdColour);
    addAndMakeVisible(thresholdSlider);

    compAttachments.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompThreshold", thresholdSlider));

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 20);
    ratioSlider.setColour(juce::Slider::rotarySliderFillColourId, ratioColour);
    addAndMakeVisible(ratioSlider);

    compAttachments.ratioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompRatio", ratioSlider));

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ratioLabel);

    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 20);
    attackSlider.setColour(juce::Slider::rotarySliderFillColourId, attackColour);
    addAndMakeVisible(attackSlider);

    compAttachments.attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompAttack", attackSlider));

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel);

    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 20);
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, releaseColour);
    addAndMakeVisible(releaseSlider);

    compAttachments.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompRelease", releaseSlider));

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);

    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 20);
    makeupSlider.setColour(juce::Slider::rotarySliderFillColourId, makeupColour);
    addAndMakeVisible(makeupSlider);

    compAttachments.makeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompMakeup", makeupSlider));

    makeupLabel.setText("Makeup", juce::dontSendNotification);
    makeupLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(makeupLabel);
}

CompressorComponent::~CompressorComponent()
{

}

void CompressorComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();
}

void CompressorComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    const int numKnobs = 5;
    auto knobWidth = area.getWidth() / numKnobs;
    const int knobSize = 100;
    const int labelHeight = 20;
    auto threshArea = area.removeFromLeft(knobWidth);
    thresholdLabel.setBounds(threshArea.removeFromTop(labelHeight));
    thresholdSlider.setBounds(threshArea.withSizeKeepingCentre(knobSize, knobSize));
    auto ratioArea = area.removeFromLeft(knobWidth);
    ratioLabel.setBounds(ratioArea.removeFromTop(labelHeight));
    ratioSlider.setBounds(ratioArea.withSizeKeepingCentre(knobSize, knobSize));
    auto attackArea = area.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackArea.removeFromTop(labelHeight));
    attackSlider.setBounds(attackArea.withSizeKeepingCentre(knobSize, knobSize));
    auto releaseArea = area.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseArea.removeFromTop(labelHeight));
    releaseSlider.setBounds(releaseArea.withSizeKeepingCentre(knobSize, knobSize));
    auto makeupArea = area;
    makeupLabel.setBounds(makeupArea.removeFromTop(labelHeight));
    makeupSlider.setBounds(makeupArea.withSizeKeepingCentre(knobSize, knobSize));
}

