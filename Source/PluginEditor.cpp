#include "PluginEditor.h"

StratomasterAudioProcessorEditor::StratomasterAudioProcessorEditor(StratomasterAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    tabs.addTab("EQ", juce::Colours::lightgrey, &eqComponent, false);
    tabs.addTab("Compressor", juce::Colours::grey, &compressorComp, false);
    tabs.addTab("Imager", juce::Colours::grey, &imagerComp, false);
    tabs.addTab("Maximizer", juce::Colours::grey, &maximizerComp, false);

    addAndMakeVisible(tabs);
    setResizable(true, true);
    setResizeLimits(600, 400, 2000, 1400);
    setSize(1000, 500);
}

StratomasterAudioProcessorEditor::~StratomasterAudioProcessorEditor()
{
}

void StratomasterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void StratomasterAudioProcessorEditor::resized()
{
    tabs.setBounds(getLocalBounds());
}
