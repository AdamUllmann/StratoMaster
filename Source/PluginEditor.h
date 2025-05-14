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
    SettingsPanel()
    {
        // example controls—you can replace these with your real settings
        addAndMakeVisible(optionA);
        optionA.setButtonText("Global Option A");

        addAndMakeVisible(optionB);
        optionB.setButtonText("Global Option B");
    }

    void paint(juce::Graphics& g) override
    {
        // draw a semi-transparent background
        g.fillAll(juce::Colours::black.withAlpha(0.8f));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
    }

    void resized() override
    {
        // stack two buttons vertically
        auto r = getLocalBounds().reduced(10);
        optionA.setBounds(r.removeFromTop(30));
        r.removeFromTop(10);
        optionB.setBounds(r.removeFromTop(30));
    }

private:
    juce::TextButton optionA, optionB;
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

    GearButton settingsButton;
    SettingsPanel settingsPanel;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessorEditor)
};
