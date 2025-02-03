/*
  ==============================================================================

    CompressorComponent.h
    Created: 3 Feb 2025 4:05:33am
    Author:  Adam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CompressorAttachments
{
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupAttachment;
};

//==============================================================================
class CompressorComponent : public juce::Component
{
public:
    CompressorComponent(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider makeupSlider;
    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label makeupLabel;
    CompressorAttachments compAttachments;
    juce::AudioProcessorValueTreeState& apvtsRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};

