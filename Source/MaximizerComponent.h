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
class MaximizerComponent : public juce::Component
{
public:
    MaximizerComponent(juce::AudioProcessorValueTreeState& apvts);
    ~MaximizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider thresholdSlider;
    juce::Slider ceilingSlider;

    juce::Slider releaseSlider;

    juce::Label thresholdLabel;
    juce::Label ceilingLabel;
    juce::Label releaseLabel;

    MaximizerAttachments attachments;
    juce::AudioProcessorValueTreeState& apvtsRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaximizerComponent)
};


