/*
  ==============================================================================

    MaximizerComponent.h
    Created: 17 Feb 2025 10:36:07pm
    Author:  Adam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct MaximizerAttachments
{
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
};

//==============================================================================
class MaximizerComponent : public juce::Component, private juce::Timer
{
public:
    MaximizerComponent(StratomasterAudioProcessor& proc, juce::AudioProcessorValueTreeState& apvts);
    ~MaximizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    juce::Slider thresholdSlider;
    juce::Slider ceilingSlider;

    juce::Slider releaseSlider;

    juce::Label thresholdLabel;
    juce::Label ceilingLabel;
    juce::Label releaseLabel;

    juce::Rectangle<int> meterRect;

    MaximizerAttachments attachments;
    juce::AudioProcessorValueTreeState& apvtsRef;
    StratomasterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaximizerComponent)
};


