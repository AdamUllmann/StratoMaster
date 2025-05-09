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

    juce::Rectangle<int> leftMeterRect, rightMeterRect;

    float outerPeakLeft = -60.0f,
        outerPeakRight = -60.0f;
    float markerPeakLeft = -60.0f,
        markerPeakRight = -60.0f;

    // how long since last new peak (seconds)
    double holdTimeLeft = 0.0,
        holdTimeRight = 0.0;

    // time of last timer tick (seconds)
    double lastTimerTime = juce::Time::getMillisecondCounterHiRes() * 0.001;

    // constants
    static constexpr double holdDuration = 2.0;   // seconds before marker decays
    static constexpr float  decayRateDbPerSec = 20.0f; // dB/sec decay speed

    // updated signature: now takes current, smoothed, and marker dB
    void drawMeter(juce::Graphics& g,
        juce::Rectangle<int> meterRect,
        float currentDb,
        float outerDb,
        float markerDb,
        bool drawLabels) const;

    MaximizerAttachments attachments;
    juce::AudioProcessorValueTreeState& apvtsRef;
    StratomasterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaximizerComponent)
};


