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
    struct BandControls {
        juce::Slider threshold;
        juce::Slider ratio;
        juce::Slider attack;
        juce::Slider release;
        juce::Slider makeup;
        juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, makeupLabel;
    };
    BandControls lowControls, midControls, highControls;
    CompressorAttachments lowAttach, midAttach, highAttach;
    juce::AudioProcessorValueTreeState& apvtsRef;
    void addBandControls(BandControls& bc, CompressorAttachments& attach, const juce::String& prefix, juce::Colour colour);
    void layoutBand(BandControls& band, juce::Rectangle<int> area);
    juce::Rectangle<int> lowArea, midArea, highArea;
    void paintBandBox(juce::Graphics& g, juce::Rectangle<int> bandArea) const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};


