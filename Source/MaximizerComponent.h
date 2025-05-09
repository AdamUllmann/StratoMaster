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
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> preGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> postGainAttachment;
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

    juce::Slider preGainSlider, postGainSlider;
    juce::Label  preGainLabel, postGainLabel;

    juce::Rectangle<int> leftMeterRect, rightMeterRect;
    juce::Rectangle<int> preLeftMeterRect, preRightMeterRect;
    juce::Rectangle<int> postLeftMeterRect, postRightMeterRect;


    float greenStartDb = -60.0f;
    float outerPeakLeft = greenStartDb, outerPeakRight = greenStartDb;
    float markerPeakLeft = greenStartDb, markerPeakRight = greenStartDb;
    float outerPreLeft = greenStartDb, outerPreRight = greenStartDb;
    float markerPreLeft = greenStartDb, markerPreRight = greenStartDb;
    float outerPostLeft = greenStartDb, outerPostRight = greenStartDb;
    float markerPostLeft = greenStartDb, markerPostRight = greenStartDb;

    double holdTimeLeft = 0.0, holdTimeRight = 0.0;
    double holdTimePreLeft = 0.0, holdTimePreRight = 0.0;
    double holdTimePostLeft = 0.0, holdTimePostRight = 0.0;

    double lastTimerTime = juce::Time::getMillisecondCounterHiRes() * 0.001;

    static constexpr double holdDuration = 2.0;
    static constexpr float  decayRateDbPerSec = 20.0f;

    void drawMeter(juce::Graphics& g, juce::Rectangle<int> meterRect, float currentDb, float outerDb, float markerDb, bool drawLabels) const;

    MaximizerAttachments attachments;
    juce::AudioProcessorValueTreeState& apvtsRef;
    StratomasterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaximizerComponent)
};


