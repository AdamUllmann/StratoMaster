/*
  ==============================================================================

    MaximizerComponent.cpp
    Created: 17 Feb 2025 10:35:43pm
    Author:  Adam

  ==============================================================================
*/

#include "MaximizerComponent.h"

namespace
{
    const auto thresholdColour = juce::Colours::orange;
    const auto ceilingColour = juce::Colours::yellow;
    const auto releaseColour = juce::Colours::cyan;
}

//==============================================================================
MaximizerComponent::MaximizerComponent(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    // Threshold
    thresholdSlider.setSliderStyle(juce::Slider::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    thresholdSlider.setColour(juce::Slider::trackColourId, thresholdColour);
    addAndMakeVisible(thresholdSlider);

    attachments.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxThreshold", thresholdSlider));

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    // Ceiling
    ceilingSlider.setSliderStyle(juce::Slider::LinearVertical);
    ceilingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    ceilingSlider.setColour(juce::Slider::trackColourId, ceilingColour);
    addAndMakeVisible(ceilingSlider);

    attachments.ceilingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxCeiling", ceilingSlider));

    ceilingLabel.setText("Ceiling", juce::dontSendNotification);
    ceilingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ceilingLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, releaseColour);
    addAndMakeVisible(releaseSlider);

    attachments.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxRelease", releaseSlider));

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);
}

MaximizerComponent::~MaximizerComponent()
{
}

void MaximizerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(35, 35, 35));
}

void MaximizerComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto leftWidth = (int)(area.getWidth() * 0.6f);
    auto leftArea = area.removeFromLeft(leftWidth);
    auto halfWidth = leftArea.getWidth() / 2;
    auto thresholdArea = leftArea.removeFromLeft(halfWidth);
    auto labelHeight = 20;
    thresholdLabel.setBounds(thresholdArea.removeFromTop(labelHeight));
    thresholdSlider.setBounds(thresholdArea);
    auto ceilingArea = leftArea;
    ceilingLabel.setBounds(ceilingArea.removeFromTop(labelHeight));
    ceilingSlider.setBounds(ceilingArea);
    auto releaseArea = area;
    releaseLabel.setBounds(releaseArea.removeFromTop(20));
    releaseSlider.setBounds(releaseArea.withSizeKeepingCentre(200, 200));
}

