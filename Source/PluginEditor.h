#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ParametricEQComponent.h"
#include "CompressorComponent.h"
#include "MaximizerComponent.h"
#include "ImagerComponent.h"

//==============================================================================
class StratomasterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    StratomasterAudioProcessorEditor(StratomasterAudioProcessor&);
    ~StratomasterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StratomasterAudioProcessor& audioProcessor;

    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };

    ParametricEQComponent eqComponent{ audioProcessor };
    CompressorComponent   compressorComp { audioProcessor.apvts };
    ImagerComponent imagerComp{ audioProcessor, audioProcessor.apvts };
    MaximizerComponent    maximizerComp{ audioProcessor, audioProcessor.apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessorEditor)
};
