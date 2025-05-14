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
    autoEQButton.setButtonText("Auto Master");
    autoEQButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    autoEQButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    autoEQButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkred.withAlpha(0.8f));
    autoEQButton.onClick = [this]()
        {
            bool goingOn = autoEQButton.getToggleState();

            if (goingOn)
            {
                autoEQButton.setButtonText("Stop");
                audioProcessor.startAutoMaster();
                startTimerHz(10);
            }
            else
            {
                autoEQButton.setButtonText("Auto Master");
                audioProcessor.stopAutoEQ();
                audioProcessor.stopAutoMaximize();
                stopTimer();
                autoMasterStatusLabel.setText("--", juce::dontSendNotification);
            }
        };
    addAndMakeVisible(autoEQButton);

    autoMasterStatusLabel.setText("--", juce::dontSendNotification);
    autoMasterStatusLabel.setFont(juce::Font("Consolas", 16.0f, juce::Font::bold));
    autoMasterStatusLabel.setJustificationType(juce::Justification::centred);
    autoMasterStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    autoMasterStatusLabel.setColour(juce::Label::backgroundColourId, juce::Colour(30, 30, 30));
    addAndMakeVisible(autoMasterStatusLabel);
    autoMasterStatusLabel.setName("StatusLabel");

    stopTimer();

    addAndMakeVisible(settingsButton);
    settingsButton.onClick = [this]()
        {
            const bool show = !settingsPanel.isVisible();
            settingsPanel.setVisible(show);
            if (show)
                settingsPanel.setBounds(getLocalBounds().reduced(10));
        };

    addAndMakeVisible(settingsPanel);
    settingsPanel.setVisible(false);

    addMouseListener(this, /*recursive=*/ true);
    setResizable(true, true);
    setResizeLimits(750, 400, 2000, 1400);
    setSize(1000, 500);
}

void StratomasterAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.isAutoEQActive)
    {
        char c = spinnerChars[spinnerIndex++ % 4];
        autoMasterStatusLabel.setText(juce::String::charToString(c) + " Equalizing", juce::dontSendNotification);
    }
    else if (audioProcessor.isAutoMaxActive)
    {
        char c = spinnerChars[spinnerIndex++ % 4];
        autoMasterStatusLabel.setText(juce::String::charToString(c) + " Maximizing", juce::dontSendNotification);
    }
}

void StratomasterAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioProcessor)
    {
        if (!audioProcessor.isAutoEQActive && !audioProcessor.isAutoMaxActive)
        {
            autoEQButton.setToggleState(false, juce::dontSendNotification);
            autoEQButton.setButtonText("Auto Master");
            stopTimer();
            autoMasterStatusLabel.setText("--", juce::dontSendNotification);
        }
    }
}

StratomasterAudioProcessorEditor::~StratomasterAudioProcessorEditor()
{
    stopTimer();
    audioProcessor.removeChangeListener(this);
    setLookAndFeel(nullptr);
    removeMouseListener(this);
}

void StratomasterAudioProcessorEditor::mouseEnter(const juce::MouseEvent& e) {
    auto* c = e.eventComponent;
    if (dynamic_cast<juce::Button*> (c) || dynamic_cast<juce::ComboBox*> (c) || dynamic_cast<juce::TextEditor*> (c)) {
        c->setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
}

void StratomasterAudioProcessorEditor::mouseExit(const juce::MouseEvent& e) {
    e.eventComponent->setMouseCursor(juce::MouseCursor::NormalCursor);
}

void StratomasterAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(30, 30, 30), 0, 0, juce::Colour(30, 30, 30), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(50, 50, 50));
    gradient.addColour(0.5, juce::Colour(70, 70, 70));
    gradient.addColour(0.8, juce::Colour(50, 50, 50));
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
    int labelWidth = 180;
    autoMasterStatusLabel.setBounds(topBar.getCentreX() - labelWidth / 2, topBar.getY(), labelWidth, topBarHeight);
    int gearSize = topBarHeight;
    settingsButton.setBounds(autoEQButton.getRight()-8, topBar.getY(), gearSize, gearSize);
    if (settingsPanel.isVisible())
        settingsPanel.setBounds(getLocalBounds().reduced(10));
}
