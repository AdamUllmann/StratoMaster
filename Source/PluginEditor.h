#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "ParametricEQComponent.h"
#include "CompressorComponent.h"
#include "MaximizerComponent.h"
#include "ImagerComponent.h"
#include "SettingsComponent.h"

//==============================================================================
class StratomasterAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::ChangeListener, private juce::Timer, private juce::MouseListener
{
public:
    StratomasterAudioProcessorEditor(StratomasterAudioProcessor&);
    ~StratomasterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

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
