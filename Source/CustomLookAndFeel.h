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
    void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    void drawLabel(juce::Graphics& g, juce::Label& label) override;
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
    int getTabButtonOverlap(int) override { return -1; }
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool isMouseOverButton, bool isButtonDown) override;
};

class GearButton : public juce::Button {
public:
    GearButton() : Button("Settings") {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void paintButton(juce::Graphics& g, bool isMouseOver, bool isButtonDown) override {
        auto area = getLocalBounds().toFloat().reduced(2.0f);
        auto radius = 1.0f;
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(area, radius);
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(area, radius, 1.0f);
        auto cx = area.getCentreX();
        auto cy = area.getCentreY();
        float R = juce::jmin(area.getWidth(), area.getHeight()) * 0.3f;
        const int    numTeeth = 6;
        const float  toothLen = R * 0.6f;
        const float  toothWidth = R * 0.5f;
        juce::Path gearPath;
        gearPath.addEllipse(cx - R, cy - R, R * 2.0f, R * 2.0f);
        for (int i = 0; i < numTeeth; ++i) {
            float angle = juce::MathConstants<float>::twoPi * i / (float)numTeeth;
            juce::Path tooth;
            tooth.addRectangle(R,-toothWidth * 0.5f,toothLen,toothWidth);
            tooth.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
            gearPath.addPath(tooth);
        }
        g.setColour(isMouseOver ? juce::Colours::white : juce::Colours::lightgrey);
        g.fillPath(gearPath);
        float innerR = R * 0.5f;
        g.setColour(juce::Colours::black);
        g.fillEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);
    }
};