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
    // low
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompLowThreshold", "MBCompLowThreshold",
        juce::NormalisableRange<float>(-60.f, 0.f, 0.1f),
        0.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompLowRatio", "MBCompLowRatio",
        juce::NormalisableRange<float>(1.f, 20.f, 0.1f),
        1.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompLowAttack", "MBCompLowAttack",
        juce::NormalisableRange<float>(1.f, 200.f, 1.f),
        20.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompLowRelease", "MBCompLowRelease",
        juce::NormalisableRange<float>(10.f, 1000.f, 1.f),
        200.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompLowMakeup", "MBCompLowMakeup",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f),
        0.f
    ));

    // mid
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompMidThreshold", "MBCompMidThreshold",
        juce::NormalisableRange<float>(-60.f, 0.f, 0.1f),
        0.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompMidRatio", "MBCompMidRatio",
        juce::NormalisableRange<float>(1.f, 20.f, 0.1f),
        1.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompMidAttack", "MBCompMidAttack",
        juce::NormalisableRange<float>(1.f, 200.f, 1.f),
        20.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompMidRelease", "MBCompMidRelease",
        juce::NormalisableRange<float>(10.f, 1000.f, 1.f),
        200.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompMidMakeup", "MBCompMidMakeup",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f),
        0.f
    ));

    // hi
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompHighThreshold", "MBCompHighThreshold",
        juce::NormalisableRange<float>(-60.f, 0.f, 0.1f),
        0.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompHighRatio", "MBCompHighRatio",
        juce::NormalisableRange<float>(1.f, 20.f, 0.1f),
        1.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompHighAttack", "MBCompHighAttack",
        juce::NormalisableRange<float>(1.f, 200.f, 1.f),
        20.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompHighRelease", "MBCompHighRelease",
        juce::NormalisableRange<float>(10.f, 1000.f, 1.f),
        200.f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MBCompHighMakeup", "MBCompHighMakeup",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f),
        0.f
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

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "PreGain", "Pre Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "PostGain", "Post Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f
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

    // MULTIBAND COMPRESSOR
    crossover1Low_L.prepare(spec);
    crossover1Low_R.prepare(spec);
    crossover1High_L.prepare(spec);
    crossover1High_R.prepare(spec);

    crossover2Low_L.prepare(spec);
    crossover2Low_R.prepare(spec);
    crossover2High_L.prepare(spec);
    crossover2High_R.prepare(spec);

    lowComp.prepare(spec);
    midComp.prepare(spec);
    highComp.prepare(spec);
    lowMakeup.prepare(spec);
    midMakeup.prepare(spec);
    highMakeup.prepare(spec);

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
    autoEQStartTime = juce::Time::getMillisecondCounterHiRes() * 0.001;
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

void StratomasterAudioProcessor::updateMultibandCompressorParams()
{
    // ====== LOW ======
    float lowThresh = apvts.getRawParameterValue("MBCompLowThreshold")->load();
    float lowRatio = apvts.getRawParameterValue("MBCompLowRatio")->load();
    float lowAttack = apvts.getRawParameterValue("MBCompLowAttack")->load();
    float lowRelease = apvts.getRawParameterValue("MBCompLowRelease")->load();
    float lowMakeupGain = apvts.getRawParameterValue("MBCompLowMakeup")->load();
    lowComp.setThreshold(lowThresh);
    lowComp.setRatio(lowRatio);
    lowComp.setAttack(lowAttack);
    lowComp.setRelease(lowRelease);
    lowMakeup.setGainDecibels(lowMakeupGain);

    // ====== MID ======
    float midThresh = apvts.getRawParameterValue("MBCompMidThreshold")->load();
    float midRatio = apvts.getRawParameterValue("MBCompMidRatio")->load();
    float midAttack = apvts.getRawParameterValue("MBCompMidAttack")->load();
    float midRelease = apvts.getRawParameterValue("MBCompMidRelease")->load();
    float midMakeupGain = apvts.getRawParameterValue("MBCompMidMakeup")->load();
    midComp.setThreshold(midThresh);
    midComp.setRatio(midRatio);
    midComp.setAttack(midAttack);
    midComp.setRelease(midRelease);
    midMakeup.setGainDecibels(midMakeupGain);

    // ====== HIGH ======
    float hiThresh = apvts.getRawParameterValue("MBCompHighThreshold")->load();
    float hiRatio = apvts.getRawParameterValue("MBCompHighRatio")->load();
    float hiAttack = apvts.getRawParameterValue("MBCompHighAttack")->load();
    float hiRelease = apvts.getRawParameterValue("MBCompHighRelease")->load();
    float hiMakeupGain = apvts.getRawParameterValue("MBCompHighMakeup")->load();
    highComp.setThreshold(hiThresh);
    highComp.setRatio(hiRatio);
    highComp.setAttack(hiAttack);
    highComp.setRelease(hiRelease);
    highMakeup.setGainDecibels(hiMakeupGain);
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

    // ============ COMPRESSORS =============
    juce::AudioBuffer<float> lowBuf(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> midBuf(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> highBuf(buffer.getNumChannels(), buffer.getNumSamples());
    lowBuf.makeCopyOf(buffer);
    midBuf.makeCopyOf(buffer);
    highBuf.makeCopyOf(buffer);
    float crossFreq1 = 250.0f;
    float crossFreq2 = 2000.0f;
    {
        crossover1Low_L.setCutoffFrequency(crossFreq1);
        crossover1Low_R.setCutoffFrequency(crossFreq1);
        crossover1High_L.setCutoffFrequency(crossFreq1);
        crossover1High_R.setCutoffFrequency(crossFreq1);

        {
            juce::dsp::AudioBlock<float> block(lowBuf);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            crossover1Low_L.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
            crossover1Low_L.process(ctx);
        }
        {
            juce::dsp::AudioBlock<float> block(midBuf);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            crossover1High_L.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            crossover1High_L.process(ctx);
        }
    }
    {
        crossover2Low_L.setCutoffFrequency(crossFreq2);
        crossover2Low_R.setCutoffFrequency(crossFreq2);
        crossover2High_L.setCutoffFrequency(crossFreq2);
        crossover2High_R.setCutoffFrequency(crossFreq2);
        {
            juce::dsp::AudioBlock<float> block(midBuf);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            crossover2Low_L.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
            crossover2Low_L.process(ctx);
        }
        {
            juce::dsp::AudioBlock<float> block(highBuf);
            juce::dsp::ProcessContextReplacing<float> ctx(block);
            crossover2High_L.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            crossover2High_L.process(ctx);
        }
    }
    updateMultibandCompressorParams();
    {
        juce::dsp::AudioBlock<float> block(lowBuf);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        lowComp.process(ctx);
        lowMakeup.process(ctx);
    }
    {
        juce::dsp::AudioBlock<float> block(midBuf);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        midComp.process(ctx);
        midMakeup.process(ctx);
    }
    {
        juce::dsp::AudioBlock<float> block(highBuf);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        highComp.process(ctx);
        highMakeup.process(ctx);
    }
    buffer.clear();
    int numCh = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < numCh; ++ch) {
        auto* out = buffer.getWritePointer(ch);
        auto* lowp = lowBuf.getReadPointer(ch);
        auto* midp = midBuf.getReadPointer(ch);
        auto* hip = highBuf.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            out[i] = lowp[i] + midp[i] + hip[i];
        }
    }

    float thresholdDb = apvts.getRawParameterValue("MaxThreshold")->load();
    float ceilingDb = apvts.getRawParameterValue("MaxCeiling")->load();
    float maxRelease = apvts.getRawParameterValue("MaxRelease")->load();
    auto nSamps = buffer.getNumSamples();
    float maxPreL = 0.0f;
    float maxPreR = 0.0f;
    float preDb = apvts.getRawParameterValue("PreGain")->load();
    float preLin = juce::Decibels::decibelsToGain(preDb);
    buffer.applyGain(preLin);

    if (buffer.getNumChannels() > 0)
        for (int i = 0; i < nSamps; ++i)
            maxPreL = std::max(maxPreL, std::fabs(buffer.getReadPointer(0)[i]));
    if (buffer.getNumChannels() > 1)
        for (int i = 0; i < nSamps; ++i)
            maxPreR = std::max(maxPreR, std::fabs(buffer.getReadPointer(1)[i]));

    currentPreGainPeakLeft = (maxPreL > 1e-6f) ? juce::Decibels::gainToDecibels(maxPreL) : -100.0f;
    currentPreGainPeakRight = (maxPreR > 1e-6f) ? juce::Decibels::gainToDecibels(maxPreR) : -100.0f;

    float makeupLin = juce::Decibels::decibelsToGain(-thresholdDb); // iZotope loudness boost
    buffer.applyGain(makeupLin);
    limiter.setThreshold(ceilingDb);
    limiter.setRatio(100.0f);
    limiter.setAttack(1.0f);
    limiter.setRelease(maxRelease);
    limiter.process(context);

    float linCeil = juce::Decibels::decibelsToGain(ceilingDb);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] = juce::jlimit(-linCeil, linCeil, data[i]);
    }

    float postDb = apvts.getRawParameterValue("PostGain")->load();
    float postLin = juce::Decibels::decibelsToGain(postDb);
    buffer.applyGain(postLin);

    float maxPostL = 0.0f, maxPostR = 0.0f;
    if (buffer.getNumChannels() > 0)
        for (int i = 0; i < nSamps; ++i)
            maxPostL = std::max(maxPostL, std::fabs(buffer.getReadPointer(0)[i]));
    if (buffer.getNumChannels() > 1)
        for (int i = 0; i < nSamps; ++i)
            maxPostR = std::max(maxPostR, std::fabs(buffer.getReadPointer(1)[i]));

    currentPostGainPeakLeft = (maxPostL > 1e-6f) ? juce::Decibels::gainToDecibels(maxPostL) : -100.0f;
    currentPostGainPeakRight = (maxPostR > 1e-6f) ? juce::Decibels::gainToDecibels(maxPostR) : -100.0f;

    float linearCeiling = juce::Decibels::decibelsToGain(ceilingDb);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* bufData = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            if (bufData[s] > linearCeiling) bufData[s] = linearCeiling;
            if (bufData[s] < -linearCeiling) bufData[s] = -linearCeiling;
        }
    }

    float maxAmpL = 0.0f, maxAmpR = 0.0f;

    if (buffer.getNumChannels() > 0)
    {
        auto* rL = buffer.getReadPointer(0);
        for (int i = 0; i < nSamps; ++i)
            maxAmpL = std::max(maxAmpL, std::fabs(rL[i]));
    }

    if (buffer.getNumChannels() > 1)
    {
        auto* rR = buffer.getReadPointer(1);
        for (int i = 0; i < nSamps; ++i)
            maxAmpR = std::max(maxAmpR, std::fabs(rR[i]));
    }

    // convert to dB (floor at –100 dB)
    currentMaximizerPeakLeft = (maxAmpL > 0.000001f)
        ? juce::Decibels::gainToDecibels(maxAmpL)
        : -100.0f;

    currentMaximizerPeakRight = (maxAmpR > 0.000001f) ? juce::Decibels::gainToDecibels(maxAmpR) : -100.0f;

    // ============= Imager DSP =============

    float width = apvts.getRawParameterValue("ImagerWidth")->load();
    if (buffer.getNumChannels() < 2) {
        return;
    }
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
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
            double now = juce::Time::getMillisecondCounterHiRes() * 0.001;
            if (now - autoEQStartTime >= autoEQDuration) {
                stopAutoEQ();
            }
            else {
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
