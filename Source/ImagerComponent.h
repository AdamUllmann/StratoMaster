/*
  ==============================================================================

    ImagerComponent.h
    Created: 24 Feb 2025 2:02:57pm
    Author:  Adam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct ImagerAttachments
{
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoizeAttachment;
};

//==============================================================================
class ImagerComponent : public juce::Component, private juce::Timer
{
public:
    ImagerComponent(StratomasterAudioProcessor& processor,
        juce::AudioProcessorValueTreeState& apvts);
    ~ImagerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    StratomasterAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::Slider widthSlider;
    juce::Slider stereoizeSlider;
    juce::Label widthLabel;
    juce::Label stereoizeLabel;

    ImagerAttachments attachments;

    juce::Rectangle<int> scopeArea;

    void timerCallback() override
    {
        repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImagerComponent)
};
