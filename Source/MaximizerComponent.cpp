/*
  ==============================================================================

    MaximizerComponent.cpp
    Created: 17 Feb 2025 10:35:43pm
    Author:  Adam

  ==============================================================================
*/

#include "MaximizerComponent.h"

namespace
{
    const auto thresholdColour = juce::Colours::orange;
    const auto ceilingColour = juce::Colours::yellow;
    const auto releaseColour = juce::Colours::cyan;
}

static constexpr float greenStartDb = -60.0f;
static constexpr float yellowStartDb = -12.0f;
static constexpr float redStartDb = -3.0f;
static constexpr float topDb = 0.0f;


//==============================================================================
MaximizerComponent::MaximizerComponent(StratomasterAudioProcessor& proc,
    juce::AudioProcessorValueTreeState& apvts)
    : audioProcessor(proc),
    apvtsRef(apvts)
{
    // Threshold
    thresholdSlider.setSliderStyle(juce::Slider::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    thresholdSlider.setColour(juce::Slider::trackColourId, thresholdColour);
    addAndMakeVisible(thresholdSlider);

    attachments.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxThreshold", thresholdSlider));

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    // Ceiling
    ceilingSlider.setSliderStyle(juce::Slider::LinearVertical);
    ceilingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    ceilingSlider.setColour(juce::Slider::trackColourId, ceilingColour);
    addAndMakeVisible(ceilingSlider);

    attachments.ceilingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxCeiling", ceilingSlider));

    ceilingLabel.setText("Ceiling", juce::dontSendNotification);
    ceilingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ceilingLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, releaseColour);
    addAndMakeVisible(releaseSlider);

    attachments.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxRelease", releaseSlider));

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);

    startTimerHz(60);
}

MaximizerComponent::~MaximizerComponent()
{
}

auto dBToFraction = [](float dBVal)
    {
        float fraction = (dBVal - greenStartDb) / (topDb - greenStartDb);
        return juce::jlimit(0.0f, 1.0f, fraction);
    };

void MaximizerComponent::drawMeter(juce::Graphics& g, juce::Rectangle<int> meterRect, float currentDb, float outerDb, float markerDb, bool drawLabels) const {
    const int x = meterRect.getX();
    const int w = meterRect.getWidth();
    const int h = meterRect.getHeight();
    const int bottom = meterRect.getBottom();
    g.setColour(juce::Colours::black);
    g.fillRect(meterRect);
    {
        constexpr float alpha = 1.0f;
        float frac = dBToFraction(outerDb);
        float yStartF = dBToFraction(yellowStartDb);
        float rStartF = dBToFraction(redStartDb);
        // GREEN
        {
            int pix = int(h * std::min(frac, yStartF));
            g.setColour(juce::Colours::limegreen.withAlpha(alpha));
            g.fillRect(x, bottom - pix, w, pix);
        }
        // YELLOW
        if (frac > yStartF) {
            int lo = int(h * yStartF);
            int hi = int(h * std::min(frac, rStartF));
            g.setColour(juce::Colours::yellow.withAlpha(alpha));
            g.fillRect(x, bottom - hi, w, hi - lo);
        }
        // RED
        if (frac > rStartF) {
            int lo = int(h * rStartF);
            int hi = int(h * frac);
            g.setColour(juce::Colours::red.withAlpha(alpha));
            g.fillRect(x, bottom - hi, w, hi - lo);
        }
    }
    /*
    // instantaneous
    {
        float frac = dBToFraction(currentDb);
        float yStartF = dBToFraction(yellowStartDb);
        float rStartF = dBToFraction(redStartDb);

        int greenPix = int(h * std::min(frac, yStartF));
        g.setColour(juce::Colours::limegreen);
        g.fillRect(x, bottom - greenPix, w, greenPix);

        if (frac > yStartF)
        {
            int lo = int(h * yStartF);
            int hi = int(h * std::min(frac, rStartF));
            g.setColour(juce::Colours::yellow);
            g.fillRect(x, bottom - hi, w, hi - lo);
        }
        if (frac > rStartF)
        {
            int lo = int(h * rStartF);
            int hi = int(h * frac);
            g.setColour(juce::Colours::red);
            g.fillRect(x, bottom - hi, w, hi - lo);
        }
    } */

    // marker tick
    {
        float frac = dBToFraction(markerDb);
        int   y = bottom - int(h * frac);
        g.setColour(juce::Colours::white);
        g.drawLine((float)x, (float)y, (float)(x + w), (float)y, 2.0f);
    }

    // grid ticks
    g.setColour(juce::Colours::grey);
    for (float db = 0; db >= greenStartDb; db -= 6.0f) {
        float frac = dBToFraction(db);
        int y = bottom - int(h * frac);
        g.drawLine((float)x, (float)y, (float)(x + w), (float)y);
        if (drawLabels) {
            g.setColour(juce::Colours::white);
            g.drawFittedText(juce::String((int)db) + " dB", x - 35, y - 7, 30, 14, juce::Justification::centredRight, 1);
            g.setColour(juce::Colours::grey);
        }
    }
}

void MaximizerComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();

    auto instL = audioProcessor.getMaximizerPeakLeft();
    auto instR = audioProcessor.getMaximizerPeakRight();

    // left meter: draw ticks+labels
    drawMeter(g, leftMeterRect,
        instL, outerPeakLeft, markerPeakLeft,
        /*drawLabels=*/true);

    // right meter: ticks only
    drawMeter(g, rightMeterRect,
        instR, outerPeakRight, markerPeakRight,
        /*drawLabels=*/false);
}

void MaximizerComponent::resized() {
    auto area = getLocalBounds().reduced(10);
    auto threshArea = area.removeFromLeft((int)(area.getWidth() * 0.2f));
    thresholdLabel.setBounds(threshArea.removeFromTop(20));
    thresholdSlider.setBounds(threshArea);
    static constexpr int meterW = 50;
    leftMeterRect = area.removeFromLeft(meterW);
    rightMeterRect = area.removeFromLeft(meterW);
    auto ceilArea = area.removeFromLeft((int)(area.getWidth() * 0.25f));
    ceilingLabel.setBounds(ceilArea.removeFromTop(20));
    ceilingSlider.setBounds(ceilArea);
    releaseLabel.setBounds(area.removeFromTop(20));
    releaseSlider.setBounds(area.withSizeKeepingCentre(250, 250));
}

void MaximizerComponent::timerCallback() {
    double now = juce::Time::getMillisecondCounterHiRes() * 0.001;
    double delta = now - lastTimerTime;
    lastTimerTime = now;
    float instL = audioProcessor.getMaximizerPeakLeft();
    float instR = audioProcessor.getMaximizerPeakRight();
    if (instL > outerPeakLeft) outerPeakLeft = instL;
    else outerPeakLeft = juce::jmax(outerPeakLeft - decayRateDbPerSec * (float)delta, instL);
    if (instL > markerPeakLeft) {
        markerPeakLeft = instL;
        holdTimeLeft = 0.0;
    }
    else {
        holdTimeLeft += delta;
        if (holdTimeLeft >= holdDuration)
            markerPeakLeft = juce::jmax(markerPeakLeft - decayRateDbPerSec * (float)delta, instL);
    }
    if (instR > outerPeakRight) outerPeakRight = instR;
    else outerPeakRight = juce::jmax(outerPeakRight - decayRateDbPerSec * (float)delta, instR);
    if (instR > markerPeakRight){
        markerPeakRight = instR;
        holdTimeRight = 0.0;
    }
    else {
        holdTimeRight += delta;
        if (holdTimeRight >= holdDuration)
            markerPeakRight = juce::jmax(markerPeakRight - decayRateDbPerSec * (float)delta, instR);
    }
    repaint();
}