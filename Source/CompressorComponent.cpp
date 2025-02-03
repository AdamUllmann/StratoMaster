/*
  ==============================================================================

    CompressorComponent.cpp
    Created: 3 Feb 2025 4:05:11am
    Author:  Adam

  ==============================================================================
*/

#include "CompressorComponent.h"

//==============================================================================
CompressorComponent::CompressorComponent(juce::AudioProcessorValueTreeState& apvts) : apvtsRef(apvts)
{
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(thresholdSlider);
    compAttachments.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompThreshold", thresholdSlider));

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(ratioSlider);

    compAttachments.ratioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompRatio", ratioSlider));

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, false);
    ratioLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ratioLabel);

    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(attackSlider);

    compAttachments.attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompAttack", attackSlider));

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel);

    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(releaseSlider);

    compAttachments.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompRelease", releaseSlider));

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);

    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(makeupSlider);

    compAttachments.makeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, "CompMakeup", makeupSlider));

    makeupLabel.setText("Makeup", juce::dontSendNotification);
    makeupLabel.attachToComponent(&makeupSlider, false);
    makeupLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(makeupLabel);
}

CompressorComponent::~CompressorComponent()
{

}

void CompressorComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker());
}

void CompressorComponent::resized()
{
    auto r = getLocalBounds();
    int knobWidth = r.getWidth() / 5;
    thresholdSlider.setBounds(r.removeFromLeft(knobWidth).reduced(8));
    ratioSlider.setBounds(r.removeFromLeft(knobWidth).reduced(8));
    attackSlider.setBounds(r.removeFromLeft(knobWidth).reduced(8));
    releaseSlider.setBounds(r.removeFromLeft(knobWidth).reduced(8));
    makeupSlider.setBounds(r.reduced(8));
}
