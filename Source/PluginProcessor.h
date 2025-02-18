#pragma once

#include <JuceHeader.h>

//==============================================================================
class StratomasterAudioProcessor : public juce::AudioProcessor
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StratomasterAudioProcessor)
};
