#include "PluginEditor.h"

StratomasterAudioProcessorEditor::StratomasterAudioProcessorEditor(StratomasterAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    tabs.addTab("EQ", juce::Colours::grey, &eqComponent, false);
    tabs.addTab("Compressor", juce::Colours::grey, &compressorComp, false);
    tabs.addTab("Imager", juce::Colours::grey, &imagerComp, false);
    tabs.addTab("Maximizer", juce::Colours::grey, &maximizerComp, false);
    addAndMakeVisible(tabs);

    stratomasterLogo.setText("Stratomaster", juce::dontSendNotification);
    juce::Font fancyFont("Monotype Corsiva", 26.0f, juce::Font::italic);
    stratomasterLogo.setFont(fancyFont);
    stratomasterLogo.setJustificationType(juce::Justification::centredRight);
    stratomasterLogo.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(stratomasterLogo);

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

    int topBarHeight = 30;
    int logoWidth = 150;
    int logoHeight = 30;
    int offsetFromTop = 0;
    int offsetFromRight = 10;
    stratomasterLogo.setBounds( getWidth() - logoWidth - offsetFromRight, offsetFromTop, logoWidth, logoHeight);
}
