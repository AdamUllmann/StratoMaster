
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class SettingsPanel : public juce::Component {
public:
    static constexpr int numBands = 8;
    SettingsPanel(juce::AudioProcessorValueTreeState& vts);
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    juce::AudioProcessorValueTreeState& state;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;
    struct ResponseCurveComponent : public juce::Component, private juce::Timer {
        ResponseCurveComponent(juce::OwnedArray<juce::Slider>& s);
        void paint(juce::Graphics& g) override;
        void timerCallback() override;
        juce::OwnedArray<juce::Slider>& sliders;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurveComponent)
    };
    std::unique_ptr<ResponseCurveComponent> responseDisplay;
    juce::OwnedArray<juce::TextButton> presets;
    juce::TextButton closeButton{ "X" };
    juce::TextButton resetButton{ "Reset" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};

#pragma once
