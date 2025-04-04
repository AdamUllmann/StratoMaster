#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "ParametricEQComponent.h"
#include "CompressorComponent.h"
#include "MaximizerComponent.h"
#include "ImagerComponent.h"

//==============================================================================
class StratomasterAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::ChangeListener
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
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessorEditor)
};
