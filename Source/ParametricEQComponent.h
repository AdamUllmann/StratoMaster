#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

struct EQBandAttachments
{
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
};

//==============================================================================
class ParametricEQComponent : public juce::Component,
    private juce::Timer,
    private juce::AudioProcessorValueTreeState::Listener,
    private juce::AsyncUpdater
{
public:
    ParametricEQComponent(StratomasterAudioProcessor& p);
    ~ParametricEQComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    StratomasterAudioProcessor& audioProcessor;

    static constexpr int numBands = 8;

    juce::Slider freqSliders[numBands];
    juce::Slider gainSliders[numBands];
    juce::Slider qSliders[numBands];

    juce::Label freqLabels[numBands];
    juce::Label gainLabels[numBands];
    juce::Label qLabels[numBands];
    juce::Label bandNumberLabels[8];

    juce::Label freqHeaderLabel;
    juce::Label resHeaderLabel;

    EQBandAttachments bandAttachments[numBands];

    juce::ComboBox filterTypeBoxes[numBands];

    struct BandHandle
    {
        juce::Point<float> centre;
        int bandIndex;
    };

    BandHandle bandHandles[numBands];
    int draggingHandleIndex = -1;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void handleAsyncUpdate() override;

    void updateHandlePositions();

    void updateSlidersFromHandle(int handleIndex, float newFreq, float newGain);

    juce::Rectangle<int> getGraphBounds() const;

    float frequencyToX(float freq) const;
    float xToFrequency(float x) const;
    float gainToY(float gainDB) const;
    float yToGain(float y) const;

    void drawBackgroundGrid(juce::Graphics& g, juce::Rectangle<int> r);
    void drawEQCurve(juce::Graphics& g, juce::Rectangle<int> r);
    void drawHandles(juce::Graphics& g);
    void drawSpectrum(juce::Graphics& g, juce::Rectangle<int> graphArea);

    juce::StringArray getAllParamIDs() const;

    void timerCallback() override;

    std::vector<float> displayMagnitudes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEQComponent)
};
