/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 31 Mar 2025 5:27:37pm
    Author:  Adam

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel() = default;
    ~CustomLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;
};
