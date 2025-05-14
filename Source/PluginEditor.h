#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "ParametricEQComponent.h"
#include "CompressorComponent.h"
#include "MaximizerComponent.h"
#include "ImagerComponent.h"

class SettingsPanel : public juce::Component
{
public:
    SettingsPanel(juce::AudioProcessorValueTreeState& vts)
        : state(vts)
    {
        for (int b = 0; b < 8; ++b) {
            auto lbl = std::make_unique<juce::Label>();
            lbl->setText("Band " + juce::String(b + 1), juce::dontSendNotification);
            lbl->setColour(juce::Label::textColourId, juce::Colours::white);
            addAndMakeVisible(*lbl);
            labels.add(std::move(lbl));
            auto sl = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                juce::Slider::TextBoxRight);
            sl->setRange(-12.0, 12.0, 0.1);
            sl->setTextValueSuffix(" dB");
            addAndMakeVisible(*sl);
            sliders.add(std::move(sl));
            attachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                state,
                "BandTarget" + juce::String(b + 1),
                *sliders.getLast()));
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.8f));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(10);
        const int rowH = 24;
        for (int i = 0; i < sliders.size(); ++i) {
            auto row = area.removeFromTop(rowH);
            labels[i]->setBounds(row.removeFromLeft(60));
            sliders[i]->setBounds(row.reduced(4, 0));
            area.removeFromTop(6);
        }
    }

private:
    juce::AudioProcessorValueTreeState& state;
    juce::OwnedArray<juce::Label> labels;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};

//==============================================================================
class StratomasterAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::ChangeListener, private juce::Timer
{
public:
    StratomasterAudioProcessorEditor(StratomasterAudioProcessor&);
    ~StratomasterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StratomasterAudioProcessor& audioProcessor;

    GearButton settingsButton;
    SettingsPanel settingsPanel{ audioProcessor.apvts };

    std::unique_ptr<CustomLookAndFeel> customLF;

    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };

    ParametricEQComponent eqComponent{ audioProcessor };
    CompressorComponent   compressorComp { audioProcessor.apvts };
    ImagerComponent imagerComp{ audioProcessor, audioProcessor.apvts };
    MaximizerComponent    maximizerComp{ audioProcessor, audioProcessor.apvts };

    juce::Label stratomasterLogo;

    juce::TextButton autoEQButton { "Auto EQ" };
    juce::Label autoMasterStatusLabel;

    int spinnerIndex = 0;
    const char* spinnerChars = "|/-\\";
    void timerCallback() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessorEditor)
};
