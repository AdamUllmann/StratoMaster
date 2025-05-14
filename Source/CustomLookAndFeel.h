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

class GearButton : public juce::Button
{
public:
    GearButton() : Button("Settings") {}

    void paintButton(juce::Graphics& g, bool /*isDown*/, bool /*isOver*/) override
    {
        auto area = getLocalBounds().toFloat().reduced(4.0f);
        auto cx = area.getCentreX();
        auto cy = area.getCentreY();
        auto radius = juce::jmin(area.getWidth(), area.getHeight()) * 0.4f;

        // central circle
        g.setColour(juce::Colours::whitesmoke);
        g.fillEllipse(cx - radius * 0.5f, cy - radius * 0.5f, radius, radius);

        // teeth
        int numTeeth = 8;
        for (int i = 0; i < numTeeth; ++i)
        {
            float angle = juce::MathConstants<float>::twoPi * i / (float)numTeeth;
            float x1 = cx + std::cos(angle) * radius;
            float y1 = cy + std::sin(angle) * radius;
            float x2 = cx + std::cos(angle) * (radius + 4.0f);
            float y2 = cy + std::sin(angle) * (radius + 4.0f);

            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
};  // <— don’t forget this semicolon!