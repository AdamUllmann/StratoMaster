#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StratomasterAudioProcessor::StratomasterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

StratomasterAudioProcessor::~StratomasterAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout StratomasterAudioProcessor::createParameterLayout()
{
    // ========== EQ PARAMETERS ==========
    static const std::array<float, 8> defaultFreqs{ 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f };

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.reserve(8 * 3); // 8 bands * (freq, gain, Q)

    for (int i = 0; i < 8; ++i)
    {
        auto bandIndexStr = juce::String(i + 1);
        // frequency
        {
            juce::String paramID = "Band" + bandIndexStr + "Freq";
            juce::String paramName = "Band " + bandIndexStr + " Freq";

            params.push_back(std::make_unique<MyFloatParameter>(
                paramID, paramName,
                juce::NormalisableRange<float>(20.0f, 20000.0f, 0.f, 0.5f),
                defaultFreqs[i]
            ));
        }
        // gain (db)
        {
            juce::String paramID = "Band" + bandIndexStr + "Gain";
            juce::String paramName = "Band " + bandIndexStr + " Gain";

            params.push_back(std::make_unique<MyFloatParameter>(
                paramID, paramName,
                juce::NormalisableRange<float>(-12.f, 12.f, 0.f, 1.f),
                0.0f
            ));
        }
        // Q
        {
            juce::String paramID = "Band" + bandIndexStr + "Q";
            juce::String paramName = "Band " + bandIndexStr + " Q";

            params.push_back(std::make_unique<MyFloatParameter>(
                paramID, paramName,
                juce::NormalisableRange<float>(0.1f, 10.f, 0.f, 1.f),
                1.0f
            ));
        }
        // type
        {
            auto filterParamID = "Band" + bandIndexStr + "FilterType";
            auto filterParamName = "Band " + bandIndexStr + " Filter Type";
            juce::StringArray filterChoices{ "Low Pass", "Peak", "High Pass", "Low Shelf", "High Shelf" };
            int defaultIndex = 1;
            if (i == 0)  // band 1
                defaultIndex = 3;
            else if (i == 7) // band 8
                defaultIndex = 4;
            params.push_back(std::make_unique<juce::AudioParameterChoice>(
                filterParamID,            // parameter ID
                filterParamName,          // parameter name
                filterChoices,            // list of filter-mode strings
                defaultIndex
            ));
        }
    }

    // ========== COMPRESSOR PARAMETERS ==========
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CompThreshold",
        "Compressor Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CompRatio",
        "Compressor Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        1.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CompAttack",
        "Compressor Attack",
        juce::NormalisableRange<float>(1.0f, 200.0f, 1.0f),
        20.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CompRelease",
        "Compressor Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f),
        200.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CompMakeup",
        "Compressor Makeup",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f
    ));

    // ========== MAXIMIZER PARAMETERS ==========
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MaxThreshold",
        "Maximizer Threshold",
        juce::NormalisableRange<float>(-20.0f, 0.0f, 0.1f),
        0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MaxCeiling",
        "Maximizer Ceiling",
        juce::NormalisableRange<float>(-20.0f, 0.0f, 0.1f),
        0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MaxRelease",
        "Maximizer Release",
        juce::NormalisableRange<float>(1.0f, 250.0f, 1.0f),
        50.0f // ms
    ));


    // ========== IMAGER PARAMETERS ==========
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ImagerWidth",
        "Imager Width",
        juce::NormalisableRange<float>(0.0f, 4.0f, 0.01f),
        1.0f // default -> normal stereo
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(   // unimplemented
        "ImagerStereoize",
        "Imager Stereoize",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f // default -> off
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String StratomasterAudioProcessor::getName() const { return JucePlugin_Name; }
bool StratomasterAudioProcessor::acceptsMidi() const { return false; }
bool StratomasterAudioProcessor::producesMidi() const { return false; }
bool StratomasterAudioProcessor::isMidiEffect() const { return false; }
double StratomasterAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int StratomasterAudioProcessor::getNumPrograms() { return 1; }
int StratomasterAudioProcessor::getCurrentProgram() { return 0; }
void StratomasterAudioProcessor::setCurrentProgram(int) {}
const juce::String StratomasterAudioProcessor::getProgramName(int) { return {}; }
void StratomasterAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void StratomasterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    for (auto& filter : peakFilters)
    {
        filter.reset();
        filter.prepare(spec);
    }

    std::fill(fifoBuffer.begin(), fifoBuffer.end(), 0.0f);
    fifoIndex = 0;
    fftDataReady.store(false);

    compressor.prepare(spec);
    limiter.prepare(spec);

    for (int b = 0; b < numBands; ++b)
    {
        diffHistory[b].resize(diffHistorySize, 0.0f);
        diffIndex[b] = 0;
    }
}

void StratomasterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StratomasterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    {
        return false;
    }

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void StratomasterAudioProcessor::startAutoEQ() {
    isAutoEQActive = true;
    blocksCloseToTarget = 0;
    static const std::array<float, 8> defaultFreqs{ 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f };
    for (int i = 0; i < 8; i++) {
        {
            juce::String freqID = "Band" + juce::String(i + 1) + "Freq";
            auto* freqParam = apvts.getParameter(freqID);
            if (freqParam != nullptr) {
                freqParam->beginChangeGesture();
                freqParam->setValueNotifyingHost(freqParam->getNormalisableRange().convertTo0to1(defaultFreqs[i]));
                freqParam->endChangeGesture();
            }
        }
        {
            juce::String gainID = "Band" + juce::String(i + 1) + "Gain";
            auto* gainParam = apvts.getParameter(gainID);
            if (gainParam != nullptr) {
                gainParam->beginChangeGesture();
                gainParam->setValueNotifyingHost(gainParam->getNormalisableRange().convertTo0to1(0.0f));
                gainParam->endChangeGesture();
            }
        }
        {
            juce::String qID = "Band" + juce::String(i + 1) + "Q";
            auto* qParam = apvts.getParameter(qID);
            if (qParam != nullptr) {
                qParam->beginChangeGesture();
                qParam->setValueNotifyingHost(qParam->getNormalisableRange().convertTo0to1(1.0f));
                qParam->endChangeGesture();
            }
        }
    }
    sendChangeMessage();
}

void StratomasterAudioProcessor::stopAutoEQ() {
    isAutoEQActive = false;
    blocksCloseToTarget = 0; // reset
    sendChangeMessage();
}

void StratomasterAudioProcessor::doAutoEQFromFFT() {
    if (!isAutoEQActive) {
        return;
    }

    std::array<float, numBands> sumMag{};
    std::array<int, numBands> count{};
    int fftLen = (int)fftMagnitudes.size();
    if (fftLen < 1) return;
    double sr = getSampleRate();
    if (sr <= 0.0) sr = 44100.0;
    for (int k = 0; k < fftLen; k++) {
        float freq = (float)(k * (sr * 0.5 / fftLen));
        float magLin = fftMagnitudes[k];
        for (int b = 0; b < numBands; ++b) {
            if (freq >= bandRanges[b].low && freq < bandRanges[b].high) {
                sumMag[b] += magLin;
                count[b]++;
                break;
            }
        }
    }
    std::array<float, numBands> bandMag{};
    for (int b = 0; b < numBands; ++b) {
        if (count[b] > 0) bandMag[b] = sumMag[b] / (float)count[b]; // linear average
        else bandMag[b] = 0.0f;  // no bins => ignore
    }
    float sumAll = 0.0f;
    int   used = 0;
    for (int b = 0; b < numBands; ++b) {
        if (bandMag[b] > 0.0f) {
            sumAll += bandMag[b];
            used++;
        }
    }
    if (used < 1) return;
    float globalMean = sumAll / (float)used;
    float globalMeanDb = juce::Decibels::gainToDecibels(globalMean + 1e-9f);
    for (int b = 0; b < numBands; ++b) {
        float bandDbVal = juce::Decibels::gainToDecibels(bandMag[b] + 1e-9f);
        if (bandDbVal < -60.0f) continue;
        float diffDb = globalMeanDb - bandDbVal;
        int idx = diffIndex[b];
        diffHistory[b][idx] = diffDb;
        diffIndex[b] = (idx + 1) % diffHistorySize;
        if (std::fabs(diffDb) < 1.5f) continue;
        float stepScaleBoost = 0.0003f;
        float stepScaleCut = 0.0008f;
        float stepScale = (diffDb > 0.f) ? stepScaleBoost : stepScaleCut;
        if (std::fabs(diffDb) >= 6.0f)
            stepScale *= 3.0f;
        float step = stepScale * diffDb;
        juce::String gainParamID = "Band" + juce::String(b + 1) + "Gain";
        if (auto* param = apvts.getParameter(gainParamID)) {
            if (auto* paramFloat = dynamic_cast<juce::AudioParameterFloat*>(param)) {
                float oldDbVal = paramFloat->get();
                float newDbVal = juce::jlimit(-12.0f, 12.0f, oldDbVal + step);
                paramFloat->beginChangeGesture();
                paramFloat->setValueNotifyingHost(paramFloat->getNormalisableRange().convertTo0to1(newDbVal));
                paramFloat->endChangeGesture();
            }
        }
    }
    bool allStable = true;
    for (int b = 0; b < numBands; ++b) {
        float sumSq = 0.0f;
        for (float d : diffHistory[b])
            sumSq += d * d;
        float meanSq = sumSq / (float)diffHistorySize;
        float rmsDiff = std::sqrt(meanSq);
        if (rmsDiff > 2.0f) {
            allStable = false;
            break;
        }
    }

    if (allStable) {
        stopAutoEQ();
        return;
    }
}

void StratomasterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    for (int bandIndex = 0; bandIndex < 8; ++bandIndex)
    {
        float freqValue = apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Freq")->load();
        float gainValue = apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Gain")->load();
        float qValue = apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Q")->load();
        int filterTypeIndex = (int)*apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "FilterType");

        juce::dsp::IIR::Coefficients<float>::Ptr coeffs;

        switch (filterTypeIndex) {
        case 0: {
            coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
                getSampleRate(),
                freqValue,
                qValue
            );
            break;
        }
        case 1: {
            coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                getSampleRate(),
                freqValue,
                qValue,
                juce::Decibels::decibelsToGain(gainValue)
            );
            break;
        }
        case 2: {
            coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
                getSampleRate(),
                freqValue,
                qValue
            );
            break;
        }
        case 3: {
            float shelfGain = juce::Decibels::decibelsToGain(gainValue);
            coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                getSampleRate(),
                freqValue,
                qValue,
                shelfGain
            );
            break;
        }
        case 4: {
            float shelfGain = juce::Decibels::decibelsToGain(gainValue);
            coeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                getSampleRate(),
                freqValue,
                qValue,
                shelfGain
            );
            break;
        }
        }
        *peakFilters[bandIndex].state = *coeffs;
    }
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    for (auto& filter : peakFilters)
        filter.process(context);
    if (totalNumInputChannels == 0)
    {
        buffer.clear();
        return;
    }

    float threshold = apvts.getRawParameterValue("CompThreshold")->load();
    float ratio = apvts.getRawParameterValue("CompRatio")->load();
    float attackMs = apvts.getRawParameterValue("CompAttack")->load();
    float releaseMs = apvts.getRawParameterValue("CompRelease")->load();
    float makeupDb = apvts.getRawParameterValue("CompMakeup")->load();

    compressor.setThreshold(threshold); // dB
    compressor.setRatio(ratio);
    compressor.setAttack(attackMs);   // ms
    compressor.setRelease(releaseMs); // ms

    compressor.process(context);

    if (std::abs(makeupDb) > 0.01f)
    {
        float linearGain = juce::Decibels::decibelsToGain(makeupDb);
        buffer.applyGain(linearGain);
    }

    auto maxThreshold = apvts.getRawParameterValue("MaxThreshold")->load();
    auto maxCeiling = apvts.getRawParameterValue("MaxCeiling")->load();
    auto maxReleaseMs = apvts.getRawParameterValue("MaxRelease")->load();

    limiter.setThreshold(maxThreshold);
    limiter.setRatio(100.0f);
    limiter.setAttack(1.0f);
    limiter.setRelease(maxReleaseMs);
    limiter.process(context);

    float linearCeiling = juce::Decibels::decibelsToGain(maxCeiling);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* bufData = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            if (bufData[s] > linearCeiling) bufData[s] = linearCeiling;
            if (bufData[s] < -linearCeiling) bufData[s] = -linearCeiling;
        }
    }

    float maxAmp = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* readPtr = buffer.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            maxAmp = std::max(maxAmp, std::fabs(readPtr[i]));
    }

    float newPeakDb = (maxAmp > 0.000001f) ? juce::Decibels::gainToDecibels(maxAmp) : -100.0f;
    currentMaximizerPeak = newPeakDb;

    // ============= Imager DSP =============

    float width = apvts.getRawParameterValue("ImagerWidth")->load();
    if (buffer.getNumChannels() < 2) {
        return;
    }
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i) // mid = (L+R)*0.5, side = (L-R)*0.5
    {
        float L = left[i];
        float R = right[i];
        float mid = 0.5f * (L + R);
        float side = 0.5f * (L - R);

        side *= width;
        left[i] = mid + side;
        right[i] = mid - side;
    }

    for (int i = 0; i < buffer.getNumSamples(); i++) {
        int index = scopeIndex.load();
        scopeBuffer[index * 2 + 0] = left[i];
        scopeBuffer[index * 2 + 1] = right[i];

        index = (index + 1) % scopeSize;
        scopeIndex.store(index);
    }

    auto* readPointer = buffer.getReadPointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        auto index = fifoIndex.load();
        fifoBuffer[index] = readPointer[i];
        index++;
        if (index == fftSize) {
            std::copy(fifoBuffer.begin(), fifoBuffer.end(), fftData.begin());
            fifoIndex.store(0);
            windowFunc.multiplyWithWindowingTable(fftData.data(), fftSize);
            forwardFFT.performRealOnlyForwardTransform(fftData.data());
            fftMagnitudes.resize(fftSize / 2, 0.0f);
            float hannComp = 2.0f / (float)fftSize;
            float offsetdB = 24.0f;
            for (int bin = 0; bin < fftSize / 2; ++bin) {
                float realPart = fftData[bin * 2 + 0];
                float imagPart = fftData[bin * 2 + 1];
                float mag = std::sqrt(realPart * realPart + imagPart * imagPart);
                mag *= hannComp; // apply hann compensation
                float binFreq = (bin * (float)getSampleRate()) / (float)fftSize;
                if (binFreq > 0.0f)
                    mag *= std::sqrt(binFreq / 20.0f); // dividing by 20.0f so the low bins (20 Hz) aren’t overly boosted. 
                // add +24 dB in magnitude
                float magDb = juce::Decibels::gainToDecibels(mag, -100.0f) + offsetdB;
                mag = juce::Decibels::decibelsToGain(magDb);
                fftMagnitudes[bin] = mag;
            }
            fftDataReady.store(true);

            // ========= AUTO EQ LOGIC =============
            if (isAutoEQActive) {
                doAutoEQFromFFT();
            }
        }
        else
        {
            fifoIndex.store(index);
        }
    }

}

//==============================================================================
bool StratomasterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* StratomasterAudioProcessor::createEditor()
{
    return new StratomasterAudioProcessorEditor(*this);
}

//==============================================================================
void StratomasterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void StratomasterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StratomasterAudioProcessor();
}
