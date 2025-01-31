#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ParametricEQComponent.h"

// Example stubs for other tabs
class CompressorComponent : public juce::Component
{
public:
    void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::darkgrey); }
    void resized() override {}
};

class ImagerComponent : public juce::Component
{
public:
    void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::darkblue); }
    void resized() override {}
};

class MaximizerComponent : public juce::Component
{
public:
    void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::darkred); }
    void resized() override {}
};

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

    // placeholders
    ParametricEQComponent eqComponent{ audioProcessor };
    CompressorComponent   compressorComp;
    ImagerComponent       imagerComp;
    MaximizerComponent    maximizerComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessorEditor)
};
