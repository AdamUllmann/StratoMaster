#pragma once

#include <JuceHeader.h>

//==============================================================================
class StratomasterAudioProcessor : public juce::AudioProcessor, public juce::ChangeBroadcaster
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    StratomasterAudioProcessor();
    ~StratomasterAudioProcessor() override;

    const std::vector<float>& getFftMagnitudes() const { return fftMagnitudes; }
    bool isFftDataReady() const { return fftDataReady; }

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    float getMaximizerPeakLeft() const { return currentMaximizerPeakLeft; }
    float getMaximizerPeakRight() const { return currentMaximizerPeakRight; }
    float getPreGainPeakLeft() const { return currentPreGainPeakLeft; }
    float getPreGainPeakRight() const { return currentPreGainPeakRight; }
    float getPostGainPeakLeft() const { return currentPostGainPeakLeft; }
    float getPostGainPeakRight() const { return currentPostGainPeakRight; }

    int getScopeSize() const { return scopeSize; }
    int getScopeIndex() const { return scopeIndex.load(); }
    const float* getScopeBuffer() const { return scopeBuffer.data(); }

    void startAutoEQ();
    void stopAutoEQ();
    void doAutoEQFromFFT();

    bool isAutoEQActive = false;

    void StratomasterAudioProcessor::updateMultibandCompressorParams();

private:
    //ugly declaration of filters for eq
    std::array<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>, 8> peakFilters;

    // ---- FIFO + FFT stuff ----
    static constexpr int fftOrder = 11;               // 2^11 = 2048
    static constexpr int fftSize = 1 << fftOrder;    // 2048

    juce::dsp::FFT forwardFFT{ fftOrder };
    juce::dsp::WindowingFunction<float> windowFunc{ fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, fftSize> fifoBuffer{};
    std::atomic<size_t> fifoIndex{ 0 };
    std::array<float, fftSize * 2> fftData{};
    std::vector<float> fftMagnitudes;
    std::atomic<bool> fftDataReady{ false };

    // MULTIBAND COMPRESSOR
    juce::dsp::LinkwitzRileyFilter<float> crossover1Low_L, crossover1Low_R;
    juce::dsp::LinkwitzRileyFilter<float> crossover1High_L, crossover1High_R;
    juce::dsp::LinkwitzRileyFilter<float> crossover2Low_L, crossover2Low_R;
    juce::dsp::LinkwitzRileyFilter<float> crossover2High_L, crossover2High_R;

    juce::dsp::Compressor<float> lowComp, midComp, highComp;
    juce::dsp::Gain<float> lowMakeup, midMakeup, highMakeup;

    juce::dsp::Compressor<float> limiter;

    float currentMaximizerPeakLeft = -100.0f;
    float currentMaximizerPeakRight = -100.0f;
    float currentPreGainPeakLeft = -100.0f, currentPreGainPeakRight = -100.0f;
    float currentPostGainPeakLeft = -100.0f, currentPostGainPeakRight = -100.0f;

    // ring buffer for L/R
    static constexpr int scopeSize = 1024;
    std::array<float, scopeSize * 2> scopeBuffer{};
    std::atomic<int> scopeIndex{ 0 };

    static constexpr int numBands = 8;
    struct BandRange { float low; float high; };
    std::array<BandRange, numBands> bandRanges
    { {
        {  20.0f,   75.0f }, // ~ 50 Hz
        {  75.0f,  150.0f }, // ~ 100 Hz
        { 150.0f,  350.0f }, // ~ 200 Hz
        { 350.0f,  750.0f }, // ~ 500 Hz
        { 750.0f, 1500.0f }, // ~ 1 kHz
        {1500.0f, 3500.0f }, // ~ 2 kHz
        {3500.0f, 7500.0f }, // ~ 5 kHz
        {7500.0f,20000.0f }  // ~ 10 kHz
    } };

    std::array<float, numBands> bandTargetDb {
        0.0f,  // ~ 50 Hz
        0.0f,  // ~ 100 Hz
        0.0f,  // ~ 200 Hz
        0.0f,  // ~ 500 Hz
        0.0f,  // ~ 1 kHz
        0.0f,  // ~ 2 kHz
        0.0f,  // ~ 5 kHz
        0.0f   // ~ 10 kHz
    };

    double autoEQStartTime = 0.0;
    static constexpr double autoEQDuration = 6.0;

    int blocksCloseToTarget = 0;
    static constexpr int freezeThresholdBlocks = 240;
    static constexpr int diffHistorySize = 64;
    std::array<std::vector<float>, numBands> diffHistory;
    std::array<int, numBands> diffIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessor)
};

class MyFloatParameter : public juce::AudioParameterFloat {
public:
    MyFloatParameter(const juce::String& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> range, float defaultValue)
        : juce::AudioParameterFloat(parameterID, parameterName, range, defaultValue) {
    }
    juce::String getText(float normalizedValue, int /*maxLength*/) const override {
        auto dBValue = range.convertFrom0to1(normalizedValue);
        return juce::String(dBValue, 2);
    }
    float getValueForText(const juce::String& text) const override {
        auto dBValue = text.getFloatValue();
        return range.convertTo0to1(dBValue);
    }
};