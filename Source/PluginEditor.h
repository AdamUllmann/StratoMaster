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
    static constexpr int numBands = 8;
    SettingsPanel(juce::AudioProcessorValueTreeState& vts)
        : state(vts)
    {
        for (int b = 0; b < numBands; b++) {
            auto lbl = std::make_unique<juce::Label>();
            lbl->setJustificationType(juce::Justification::centred);
            lbl->setColour(juce::Label::textColourId, juce::Colours::white);
            addAndMakeVisible(*lbl);
            labels.add(std::move(lbl));
            auto sl = std::make_unique<juce::Slider>(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
            sl->setRange(-12.0, 12.0, 0.1);
            sl->setTextValueSuffix(" dB");
            addAndMakeVisible(*sl);
            sliders.add(std::move(sl));
            attachments.add(std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                    state,
                    "BandTarget" + juce::String(b + 1),
                    *sliders.getLast()));
        }

        responseDisplay = std::make_unique<ResponseCurveComponent>(sliders);
        addAndMakeVisible(responseDisplay.get());

        auto makePreset = [this](const juce::String& name, std::array<float, numBands> vals)
            {
                auto btn = std::make_unique<juce::TextButton>(name);
                btn->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
                btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
                btn->onClick = [this, vals]()
                    {
                        for (int i = 0; i < numBands; ++i)
                            sliders[i]->setValue(vals[i], juce::NotificationType::sendNotification);
                    };
                addAndMakeVisible(*btn);
                presets.add(std::move(btn));
            };

        makePreset("Bass Boost", { 6,  4,  2,  0,  0,  0,  0,  0 });
        makePreset("Bright", { 0,  0,  0,  0,  0,  2,  4,  6 });
        makePreset("V-Shape", { 5,  2,  0,  0,  0,  0,  2,  5 });
        makePreset("Warm", { 0,  0,  3,  4,  3,  0,  0,  0 });
        makePreset("Mid Scoop", { 0, -4, -6,  0,  0, -6, -4,  0 });
        makePreset("Vocal Presence", { 0,  0,  0,  0,  3,  4,  3,  0 });
        makePreset("Drum Punch", { 4,  2,  0,  0,  0,  0,  0,  0 });
        makePreset("Airy Sparkle", { 0,  0,  0,  0,  0,  0,  2,  5 });

        closeButton.setButtonText("X");
        closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        closeButton.onClick = [this]() { setVisible(false); };
        addAndMakeVisible(closeButton);

        resetButton.setButtonText("Reset");
        resetButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        resetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        resetButton.onClick = [this]()
            {
                for (auto* s : sliders)
                    s->setValue(0.0, juce::NotificationType::sendNotification);
            };
        addAndMakeVisible(resetButton);
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black.withAlpha(0.8f));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
    }

    void resized() override {
        auto fullArea = getLocalBounds().reduced(10);
        auto leftArea = fullArea.removeFromLeft(fullArea.getWidth() * 3 / 4);
        auto rightArea = fullArea;
        auto topBar = rightArea.removeFromTop(30);
        auto closeB = topBar.removeFromRight(30);
        auto resetB = topBar.removeFromRight(60);
        closeButton.setBounds(closeB.reduced(2));
        resetButton.setBounds(resetB.reduced(2));
        auto curveFull = leftArea.removeFromTop(100);
        auto insetW = int(curveFull.getWidth() * 0.1f);
        auto curveW = int(curveFull.getWidth() * 0.8f);
        juce::Rectangle<int> curveArea{
            curveFull.getX() + insetW,
            curveFull.getY(),
            curveW,
            curveFull.getHeight()
        };
        responseDisplay->setBounds(curveArea.reduced(4));
        int sliderW = leftArea.getWidth() / numBands;
        for (int i = 0; i < numBands; i++) {
            auto col = leftArea.removeFromLeft(sliderW);
            sliders[i]->setBounds(col.removeFromTop(col.getHeight() - 20));
            labels[i]->setText("Band " + juce::String(i + 1), juce::dontSendNotification);
            labels[i]->setBounds(col);
        }
        int cols = 2, rows = 4;
        int btnW = (rightArea.getWidth() - (cols - 1) * 8) / cols;
        int btnH = (rightArea.getHeight() - (rows - 1) * 8) / rows;
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c)
            {
                int idx = r * cols + c;
                int x = rightArea.getX() + c * (btnW + 8);
                int y = rightArea.getY() + r * (btnH + 8);
                presets[idx]->setBounds(x, y, btnW, btnH);
            }
    }

private:
    juce::AudioProcessorValueTreeState& state;
    juce::OwnedArray<juce::Slider>     sliders;
    juce::OwnedArray<juce::Label>      labels;
    juce::OwnedArray<
        juce::AudioProcessorValueTreeState::SliderAttachment> attachments;
    struct ResponseCurveComponent : public juce::Component,
        private juce::Timer
    {
        ResponseCurveComponent(juce::OwnedArray<juce::Slider>& s) : sliders(s) {
            startTimerHz(60);
        }
        void paint(juce::Graphics& g) override {
            auto area = getLocalBounds().toFloat();
            constexpr float cornerSize = 8.0f;
            g.setColour(juce::Colour(20, 20, 25));
            g.fillRoundedRectangle(area, cornerSize);
            g.setColour(juce::Colours::white.withAlpha(0.2f));
            g.drawRoundedRectangle(area, cornerSize, 1.0f);

            const float minLog = std::log10(20.0f),
                maxLog = std::log10(20000.0f);
            static constexpr float freqs[numBands] = { 50,100,200,500,1000,2000,5000,10000 };
            juce::Path p;
            for (int i = 0; i < sliders.size(); i++) {
                float normX = (std::log10(freqs[i]) - minLog) / (maxLog - minLog);
                float x = area.getX() + normX * area.getWidth();

                float db = (float)sliders[i]->getValue();  // –12…12
                float y = area.getY() + (1.0f - ((db + 12.0f) / 24.0f)) * area.getHeight();

                if (i == 0)   p.startNewSubPath(x, y);
                else          p.lineTo(x, y);
            }
            g.setColour(juce::Colour(150, 180, 200));
            g.strokePath(p, juce::PathStrokeType(2.0f));
        }
        void timerCallback() override { repaint(); }
        juce::OwnedArray<juce::Slider>& sliders;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurveComponent)
    };
    std::unique_ptr<ResponseCurveComponent> responseDisplay;
    juce::OwnedArray<juce::TextButton> presets;
    juce::TextButton closeButton{ "X" };
    juce::TextButton resetButton{ "Reset" };

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
