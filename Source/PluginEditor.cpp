#include "PluginEditor.h"

StratomasterAudioProcessorEditor::StratomasterAudioProcessorEditor(StratomasterAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    customLF.reset(new CustomLookAndFeel());
    setLookAndFeel(customLF.get());
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

    audioProcessor.addChangeListener(this);

    autoEQButton.setClickingTogglesState(true);
    autoEQButton.setButtonText("Auto EQ");
    autoEQButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey.brighter());
    autoEQButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    autoEQButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkred.withAlpha(0.8f));
    autoEQButton.onClick = [this]() {
        bool isOn = autoEQButton.getToggleState();
        if (isOn) {
            autoEQButton.setButtonText("Stop");
            audioProcessor.startAutoEQ();
        }
        else {
            autoEQButton.setButtonText("Auto EQ");
            audioProcessor.stopAutoEQ();
        }
    };
    addAndMakeVisible(autoEQButton);

    setResizable(true, true);
    setResizeLimits(600, 400, 2000, 1400);
    setSize(1000, 500);
}

void StratomasterAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioProcessor) {
        if (!audioProcessor.isAutoEQActive) {
            autoEQButton.setToggleState(false, juce::NotificationType::dontSendNotification);
            autoEQButton.setButtonText("Auto EQ");
        }
    }
}

StratomasterAudioProcessorEditor::~StratomasterAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void StratomasterAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(50, 50, 50), 0, 0, juce::Colour(50, 50, 50), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(70, 70, 70));
    gradient.addColour(0.5, juce::Colour(90, 90, 90));
    gradient.addColour(0.8, juce::Colour(70, 70, 70));
    g.setGradientFill(gradient);
    g.fillAll();
}

void StratomasterAudioProcessorEditor::resized()
{
    tabs.setBounds (getLocalBounds());
    auto topBarHeight = 30;
    auto topBar = getLocalBounds().removeFromTop (topBarHeight);
    int logoWidth  = 150;
    int offsetFromRight = 10;
    stratomasterLogo.setBounds (topBar.getRight() - logoWidth - offsetFromRight, topBar.getY(), logoWidth, topBarHeight);
    int buttonWidth = 120;
    autoEQButton.setBounds (stratomasterLogo.getX() - (buttonWidth + 10), topBar.getY(), buttonWidth, topBarHeight);
}
