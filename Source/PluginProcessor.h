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

    float getMaximizerPeak() const { return currentMaximizerPeak; }

    int getScopeSize() const { return scopeSize; }
    int getScopeIndex() const { return scopeIndex.load(); }
    const float* getScopeBuffer() const { return scopeBuffer.data(); }

    void startAutoEQ();
    void stopAutoEQ();
    void doAutoEQFromFFT();

    bool isAutoEQActive = false;

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

    juce::dsp::Compressor<float> compressor;
    juce::dsp::Compressor<float> limiter;

    float currentMaximizerPeak = -100.0f;

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

    int blocksCloseToTarget = 0;
    static constexpr int freezeThresholdBlocks = 240;
    static constexpr int diffHistorySize = 64;
    std::array<std::vector<float>, numBands> diffHistory;
    std::array<int, numBands> diffIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessor)
};