#include "ParametricEQComponent.h"

static const std::array<juce::Colour, 8> bandColours
{
    juce::Colour(0xFFBF00FF), // neon purple
    juce::Colour(0xFFFF00BF), // neon pink
    juce::Colour(0xFFFF3333), // neon red
    juce::Colour(0xFFFF6600), // neon orange
    juce::Colour(0xFFFFFF33), // neon yellow
    juce::Colour(0xFF33FF33), // neon green
    juce::Colour(0xFF33FFFF), // neon cyan
    juce::Colour(0xFF3366FF)  // neon blue
};

//==============================================================================
ParametricEQComponent::ParametricEQComponent(StratomasterAudioProcessor& p)
    : audioProcessor(p)
{
    for (int i = 0; i < numBands; ++i)
    {
        // --- frequency knobs ---
        freqSliders[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        freqSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
        addAndMakeVisible(freqSliders[i]);
        freqSliders[i].setColour(juce::Slider::rotarySliderFillColourId, bandColours[i]);

        auto freqParamID = "Band" + juce::String(i + 1) + "Freq";
        bandAttachments[i].freqAttachment.reset(
            new juce::AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.apvts, freqParamID, freqSliders[i]
            )
        );

        freqLabels[i].setText("F" + juce::String(i + 1), juce::dontSendNotification);
        freqLabels[i].attachToComponent(&freqSliders[i], false);
        freqLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(freqLabels[i]);

        // --- gain knobs ---
        gainSliders[i].setSliderStyle(juce::Slider::LinearVertical);
        gainSliders[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        gainSliders[i].setNumDecimalPlacesToDisplay(2);

        addAndMakeVisible(gainSliders[i]);
        gainSliders[i].setColour(juce::Slider::trackColourId, bandColours[i]);

        auto gainParamID = "Band" + juce::String(i + 1) + "Gain";
        bandAttachments[i].gainAttachment.reset(
            new juce::AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.apvts, gainParamID, gainSliders[i]
            )
        );

        gainLabels[i].setText("G" + juce::String(i + 1), juce::dontSendNotification);
        gainLabels[i].attachToComponent(&gainSliders[i], false);
        gainLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(gainLabels[i]);

        // --- Q knobs ---
        qSliders[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        qSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);
        addAndMakeVisible(qSliders[i]);
        qSliders[i].setColour(juce::Slider::rotarySliderFillColourId, bandColours[i]);

        auto qParamID = "Band" + juce::String(i + 1) + "Q";
        bandAttachments[i].qAttachment.reset(
            new juce::AudioProcessorValueTreeState::SliderAttachment(
                audioProcessor.apvts, qParamID, qSliders[i]
            )
        );

        qLabels[i].setText("Q" + juce::String(i + 1), juce::dontSendNotification);
        qLabels[i].attachToComponent(&qSliders[i], false);
        qLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(qLabels[i]);

        auto filterTypeParamID = "Band" + juce::String(i + 1) + "FilterType";

        addAndMakeVisible(filterTypeBoxes[i]);

        filterTypeBoxes[i].addItem("Low Pass", 1);
        filterTypeBoxes[i].addItem("Peak", 2);
        filterTypeBoxes[i].addItem("High Pass", 3);
        filterTypeBoxes[i].addItem("Low Shelf", 4);
        filterTypeBoxes[i].addItem("High Shelf", 5);

        bandAttachments[i].filterTypeAttachment.reset(
            new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
                audioProcessor.apvts,
                filterTypeParamID,
                filterTypeBoxes[i]
            )
        );

        bandHandles[i].bandIndex = i;
        bandHandles[i].centre = { 0.f, 0.f };
    }

    auto paramIDs = getAllParamIDs();
    for (auto& paramID : paramIDs)
        p.apvts.addParameterListener(paramID, this);

    addMouseListener(this, true);
    updateHandlePositions();

    startTimerHz(60);
}

ParametricEQComponent::~ParametricEQComponent()
{
    auto paramIDs = getAllParamIDs();
    for (auto& paramID : paramIDs)
        audioProcessor.apvts.removeParameterListener(paramID, this);
    stopTimer();
}

void ParametricEQComponent::timerCallback()
{
    if (audioProcessor.isFftDataReady())
    {
        auto& mag = audioProcessor.getFftMagnitudes();
        if (displayMagnitudes.size() != mag.size())
            displayMagnitudes.resize(mag.size());

        // smoothing
        for (size_t i = 0; i < mag.size(); ++i)
            displayMagnitudes[i] = 0.8f * displayMagnitudes[i] + 0.2f * mag[i];

        repaint();
    }
}

//==============================================================================
void ParametricEQComponent::paint(juce::Graphics& g)
{
    auto graphArea = getGraphBounds();
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();
    g.setColour(juce::Colour(25, 25, 25));
    g.fillRect(graphArea);
    drawSpectrum(g, graphArea);
    drawBackgroundGrid(g, graphArea);
    drawEQCurve(g, graphArea);
    drawHandles(g);
}

void ParametricEQComponent::drawSpectrum(juce::Graphics& g, juce::Rectangle<int> graphArea)
{
    // we have displayMagnitudes[] of size e.g. 1024, representing freq bins from 0..
    // we'll turn those into dB, then map freq->x, dB->y
    g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
    if (displayMagnitudes.empty()) return;

    juce::Path spectrumPath;
    bool firstPoint = true;

    auto sr = audioProcessor.getSampleRate();
    if (sr <= 0.0) sr = 44100.0;

    for (size_t binIndex = 0; binIndex < displayMagnitudes.size(); ++binIndex)
    {
        float binFreq = (float)binIndex * (float)sr / (float)(displayMagnitudes.size() * 2);

        float mag = displayMagnitudes[binIndex];
        float magDb = juce::Decibels::gainToDecibels(mag, -100.0f); // floor at -100 dB

        float x = frequencyToX(binFreq);
        x = juce::jlimit<float>((float)graphArea.getX(), (float)graphArea.getRight(), x);

        const float spectrumMinDb = -35.f;
        const float spectrumMaxDb = 35.f;
        float norm = juce::jmap(magDb, spectrumMinDb, spectrumMaxDb, 0.f, 1.f);
        float y = graphArea.getBottom() - norm * (float)graphArea.getHeight();
        y = juce::jlimit<float>((float)graphArea.getY(), (float)graphArea.getBottom(), y);

        if (firstPoint)
        {
            spectrumPath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            spectrumPath.lineTo(x, y);
        }
    }
    spectrumPath.lineTo(graphArea.getRight(), graphArea.getBottom());
    spectrumPath.lineTo(graphArea.getX(), graphArea.getBottom());
    spectrumPath.closeSubPath();
    g.fillPath(spectrumPath);
}

void ParametricEQComponent::resized() 
{
    auto bounds = getLocalBounds();
    auto controlArea = bounds.removeFromRight(300);
    updateHandlePositions();
    const int columnWidth = controlArea.getWidth() / numBands;
    const int columnHeight = controlArea.getHeight();
    for (int i = 0; i < numBands; ++i) {
        auto column = controlArea.removeFromLeft(columnWidth);
        auto sliderHeight = static_cast<int>(columnHeight * 0.5f);
        auto knobHeight = static_cast<int>(columnHeight * 0.25f);
        {
            auto gainArea = column.removeFromTop(sliderHeight);
            gainSliders[i].setBounds(gainArea);  // leftover
        }
        {
            auto freqArea = column.removeFromTop(knobHeight);
            int labelHeight = 40;
            auto freqLabelArea = freqArea.removeFromTop(labelHeight);
            freqLabels[i].setBounds(freqLabelArea.withTrimmedTop(2));
            freqSliders[i].setBounds(freqArea);
        }
        {
            auto qArea = column;
            int labelHeight = 40;
            auto qLabelArea = qArea.removeFromTop(labelHeight);
            qLabels[i].setBounds(qLabelArea.withTrimmedTop(2));
            qSliders[i].setBounds(qArea);
        }
        {
            auto filterBoxHeight = 20;
            auto filterBoxArea = column.removeFromBottom(filterBoxHeight);

            filterTypeBoxes[i].setBounds(filterBoxArea);
        }
    }
}

//==============================================================================
void ParametricEQComponent::mouseDown(const juce::MouseEvent& event)
{
    auto graphArea = getGraphBounds();
    if (!graphArea.contains(event.getPosition()))
        return;
    const float handleRadius = 6.0f;
    for (int i = 0; i < numBands; ++i)
    {
        auto distance = event.getPosition().toFloat().getDistanceFrom(bandHandles[i].centre);
        if (distance < handleRadius + 3.0f)
        {
            draggingHandleIndex = i;
            break;
        }
    }
}

void ParametricEQComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (draggingHandleIndex < 0)
        return;
    auto graphArea = getGraphBounds();
    auto pos = event.getPosition().toFloat();
    pos.x = juce::jlimit<float>((float)graphArea.getX(), (float)graphArea.getRight(), pos.x);
    pos.y = juce::jlimit<float>((float)graphArea.getY(), (float)graphArea.getBottom(), pos.y);
    float newFreq = xToFrequency(pos.x);
    float newGain = yToGain(pos.y);
    updateSlidersFromHandle(draggingHandleIndex, newFreq, newGain); // update the param for freq & gain. Q is untouched
}

void ParametricEQComponent::mouseUp(const juce::MouseEvent& /*event*/)
{
    draggingHandleIndex = -1;
}

//==============================================================================
void ParametricEQComponent::parameterChanged(const juce::String& /*parameterID*/, float /*newValue*/)
{
    triggerAsyncUpdate();
}

void ParametricEQComponent::handleAsyncUpdate()
{
    updateHandlePositions();
}

void ParametricEQComponent::updateHandlePositions()
{
    auto graphArea = getGraphBounds();
    for (int i = 0; i < numBands; ++i)
    {
        float freqVal = audioProcessor.apvts.getRawParameterValue("Band" + juce::String(i + 1) + "Freq")->load();
        float gainVal = audioProcessor.apvts.getRawParameterValue("Band" + juce::String(i + 1) + "Gain")->load();
        float x = frequencyToX(freqVal);
        float y = gainToY(gainVal);
        bandHandles[i].centre = { x, y };
    }
    repaint();
}

void ParametricEQComponent::updateSlidersFromHandle(int handleIndex, float newFreq, float newGain)
{
    freqSliders[handleIndex].setValue(newFreq, juce::sendNotificationSync);
    gainSliders[handleIndex].setValue(newGain, juce::sendNotificationSync);
}

constexpr int leftLabelMargin = 40;
constexpr int bottomLabelMargin = 20;
constexpr int topMargin = 10;
constexpr int rightMargin = 10;

//==============================================================================
juce::Rectangle<int> ParametricEQComponent::getGraphBounds() const
{
    auto total = getLocalBounds();
    total.reduce(0, 0);
    total.setWidth(total.getWidth() - 300);

    total.removeFromLeft(leftLabelMargin);
    total.removeFromBottom(bottomLabelMargin);
    total.removeFromTop(topMargin);
    total.removeFromRight(rightMargin);

    return total;
}

float ParametricEQComponent::frequencyToX(float freq) const
{
    auto r = getGraphBounds();
    const float minFreq = 20.f;
    const float maxFreq = 20000.f;
    const float logMin = std::log10(minFreq);
    const float logMax = std::log10(maxFreq);
    const float lf = std::log10(freq);
    float norm = (lf - logMin) / (logMax - logMin);
    return r.getX() + norm * (float)r.getWidth();
}

float ParametricEQComponent::xToFrequency(float x) const
{
    auto r = getGraphBounds();
    float t = (x - r.getX()) / (float)r.getWidth();
    t = juce::jlimit(0.0f, 1.0f, t);
    const float minFreq = 20.f;
    const float maxFreq = 20000.f;
    const float logMin = std::log10(minFreq);
    const float logMax = std::log10(maxFreq);
    float logF = logMin + t * (logMax - logMin);
    return std::pow(10.0f, logF);
}

float ParametricEQComponent::gainToY(float gainDB) const
{
    auto r = getGraphBounds();
    const float minGain = -12.f;
    const float maxGain = 12.f;
    float norm = (gainDB - minGain) / (maxGain - minGain);
    float y = r.getBottom() - norm * (float)r.getHeight();
    return y;
}

float ParametricEQComponent::yToGain(float y) const
{
    auto r = getGraphBounds();
    const float minGain = -12.f;
    const float maxGain = 12.f;
    float t = (r.getBottom() - y) / (float)r.getHeight();
    t = juce::jlimit(0.0f, 1.0f, t);
    return minGain + t * (maxGain - minGain);
}

//==============================================================================
void ParametricEQComponent::drawBackgroundGrid(juce::Graphics& g, juce::Rectangle<int> graphArea)
{
    // frequencies to mark:
    static const std::array<float, 10> freqLines{
        20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f
    };

    // dB lines to mark
    static const std::array<float, 5> dbLines{
        -12.f, -6.f, 0.f, 6.f, 12.f
    };

    g.setColour(juce::Colours::grey.withAlpha(0.3f));

    // frequency lines
    for (auto freq : freqLines)
    {
        float x = frequencyToX(freq);
        x = juce::jlimit<float>((float)graphArea.getX(), (float)graphArea.getRight(), x);
        
        g.drawLine(x, (float)graphArea.getY(), x, (float)graphArea.getBottom());

        // label
        juce::String labelText = (freq < 1000.f)
            ? juce::String((int)freq) + " Hz"
            : juce::String(freq / 1000.f, 1) + " kHz";

        auto fullBounds = getLocalBounds();
        auto bottomY = fullBounds.getBottom() - bottomLabelMargin / 2;
        int textWidth = 40;
        int textHeight = 14;
        juce::Rectangle<int> textRect((int)(x - textWidth * 0.5f),
            (int)(bottomY - textHeight * 0.5f),
            textWidth, textHeight);

        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.drawFittedText(labelText, textRect, juce::Justification::centred, 1);
    }

    // dB lines
    for (auto dB : dbLines)
    {
        float y = gainToY(dB);
        y = juce::jlimit<float>((float)graphArea.getY(), (float)graphArea.getBottom(), y);

        g.drawLine((float)graphArea.getX(), y, (float)graphArea.getRight(), y);

        // label
        auto fullBounds = getLocalBounds();
        int textWidth = 35;
        int textHeight = 14;
        juce::Rectangle<int> textRect(
            fullBounds.getX(), 
            (int)(y - textHeight * 0.5f),
            leftLabelMargin - 4,
            textHeight);

        juce::String labelText = juce::String((int)dB) + " dB";
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.drawFittedText(labelText, textRect, juce::Justification::centredRight, 1);
    }
}

void ParametricEQComponent::drawEQCurve(juce::Graphics& g, juce::Rectangle<int> graphArea)
{
    // default sample rate if one hasn't been set
    double sr = audioProcessor.getSampleRate();
    if (sr <= 0.0)
        sr = 44100.0;
    juce::Path eqPath;
    bool firstPoint = true;
    constexpr int numPoints = 128;
    // log-spaced frequencies
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = 20.0f * std::pow(10.0f,  // spanning 20 Hz to 20 kHz
            (float)i * (std::log10(20000.0f / 20.0f) / (numPoints - 1)));
        double totalDb = 0.0;
        for (int bandIndex = 0; bandIndex < numBands; ++bandIndex) // sum dB magnitudes for each band
        {
            float bFreq = audioProcessor.apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Freq")->load();
            float bGain = audioProcessor.apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Gain")->load();
            float bQ = audioProcessor.apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "Q")->load();
            int filterTypeIndex = (int)*audioProcessor.apvts.getRawParameterValue("Band" + juce::String(bandIndex + 1) + "FilterType");
            juce::dsp::IIR::Coefficients<double>::Ptr coeffs;
            switch (filterTypeIndex) {
            case 0:
                coeffs = juce::dsp::IIR::Coefficients<double>::makeLowPass(
                    sr, (double)bFreq, (double)bQ
                );
                break;

            case 1:
                coeffs = juce::dsp::IIR::Coefficients<double>::makePeakFilter(
                    sr,
                    (double)bFreq,
                    (double)bQ,
                    juce::Decibels::decibelsToGain((double)bGain)
                );
                break;

            case 2:
                coeffs = juce::dsp::IIR::Coefficients<double>::makeHighPass(
                    sr, (double)bFreq, (double)bQ
                );
                break;

            case 3: // Low Shelf
                coeffs = juce::dsp::IIR::Coefficients<double>::makeLowShelf(
                    sr,
                    (double)bFreq,
                    (double)bQ,
                    juce::Decibels::decibelsToGain((double)bGain)
                );
                break;

            case 4: // High Shelf
                coeffs = juce::dsp::IIR::Coefficients<double>::makeHighShelf(
                    sr,
                    (double)bFreq,
                    (double)bQ,
                    juce::Decibels::decibelsToGain((double)bGain)
                );
                break;

            default:
                coeffs = juce::dsp::IIR::Coefficients<double>::makePeakFilter(
                    sr,
                    (double)bFreq,
                    (double)bQ,
                    juce::Decibels::decibelsToGain((double)bGain)
                );
                break;
            }
            double magLin = coeffs->getMagnitudeForFrequency(freq, sr);
            double magDb = juce::Decibels::gainToDecibels((float)magLin); // convert linear magnitude to dB
            totalDb += magDb;
        }
        float overallDb = (float)totalDb;
        // convert (freq, dB) to (x, y)
        float x = frequencyToX(freq);
        x = juce::jlimit<float>((float)graphArea.getX(),
            (float)graphArea.getRight(),
            x);
        float y = gainToY(overallDb);
        y = juce::jlimit<float>((float)graphArea.getY(),
            (float)graphArea.getBottom(),
            y);
        if (firstPoint)
        {
            eqPath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            eqPath.lineTo(x, y);
        }
    }
    g.setColour(juce::Colours::white);
    g.strokePath(eqPath, juce::PathStrokeType(2.0f));
}

void ParametricEQComponent::drawHandles(juce::Graphics& g)
{
    const float radius = 8.0f;
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    for (int i = 0; i < numBands; ++i)
    {
        auto c = bandHandles[i].centre;
        g.setColour(juce::Colours::black);
        g.fillEllipse(c.x - radius, c.y - radius, radius * 2.f, radius * 2.f);
        g.setColour(bandColours[i]);
        g.drawEllipse(c.x - radius, c.y - radius, radius * 2.f, radius * 2.f, 2.0f);
        g.setColour(juce::Colours::white);
        juce::Rectangle<int> textBounds(
            (int)(c.x - radius),
            (int)(c.y - radius),
            (int)(radius * 2),
            (int)(radius * 2));
        g.drawFittedText(juce::String(i + 1),
            textBounds,
            juce::Justification::centred,
            1);
    }
}

juce::StringArray ParametricEQComponent::getAllParamIDs() const
{
    juce::StringArray ids;
    for (int i = 1; i <= numBands; ++i)
    {
        ids.add("Band" + juce::String(i) + "Freq");
        ids.add("Band" + juce::String(i) + "Gain");
        ids.add("Band" + juce::String(i) + "Q");
    }
    return ids;
}
