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
    auto verdanaBold = juce::Font("Verdana", 14.0f, juce::Font::bold);

    // Threshold
    thresholdSlider.setSliderStyle(juce::Slider::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    thresholdSlider.setColour(juce::Slider::trackColourId, thresholdColour);
    addAndMakeVisible(thresholdSlider);

    attachments.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxThreshold", thresholdSlider));

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    thresholdLabel.setFont(verdanaBold);
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
    ceilingLabel.setFont(verdanaBold);
    addAndMakeVisible(ceilingLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    releaseSlider.setColour(juce::Slider::trackColourId, releaseColour);
    addAndMakeVisible(releaseSlider);

    attachments.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "MaxRelease", releaseSlider));

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.setFont(verdanaBold);
    addAndMakeVisible(releaseLabel);

    // Pre-Gain
    preGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    preGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    preGainSlider.setColour(juce::Slider::trackColourId, juce::Colours::deepskyblue);
    preGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::skyblue);
    addAndMakeVisible(preGainSlider);
    attachments.preGainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "PreGain", preGainSlider));
    preGainLabel.setText("Pre", juce::dontSendNotification);
    preGainLabel.setJustificationType(juce::Justification::centred);
    preGainLabel.setFont(verdanaBold);
    addAndMakeVisible(preGainLabel);

    // Post-Gain
    postGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    postGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);
    postGainSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink);
    postGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::deeppink);
    addAndMakeVisible(postGainSlider);
    attachments.postGainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        apvtsRef, "PostGain", postGainSlider));
    postGainLabel.setText("Post", juce::dontSendNotification);
    postGainLabel.setJustificationType(juce::Justification::centred);
    postGainLabel.setFont(verdanaBold);
    addAndMakeVisible(postGainLabel);

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

    g.setFont(juce::Font("Verdana", 11.0f, juce::Font::bold));
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
    g.setFont(juce::Font("Verdana", 11.0f, juce::Font::bold));

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

    auto preInstL = audioProcessor.getPreGainPeakLeft();
    auto preInstR = audioProcessor.getPreGainPeakRight();
    drawMeter(g, preLeftMeterRect,
        preInstL, outerPreLeft, markerPreLeft,
        /*drawLabels=*/ true);
    drawMeter(g, preRightMeterRect,
        preInstR, outerPreRight, markerPreRight,
        /*drawLabels=*/ false);

    auto postInstL = audioProcessor.getPostGainPeakLeft();
    auto postInstR = audioProcessor.getPostGainPeakRight();
    drawMeter(g, postLeftMeterRect,
        postInstL, outerPostLeft, markerPostLeft,
        /*drawLabels=*/ true);
    drawMeter(g, postRightMeterRect,
        postInstR, outerPostRight, markerPostRight,
        /*drawLabels=*/ false);
}

void MaximizerComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    // Threshold
    auto tA = area.removeFromLeft(int(area.getWidth() * 0.2f));
    thresholdLabel.setBounds(tA.removeFromTop(20));
    thresholdSlider.setBounds(tA);

    // meters 
    constexpr int limMeterW = 50;
    constexpr int meterVMargin = 20;
    {
        auto leftA = area.removeFromLeft(limMeterW);
        leftMeterRect = leftA.withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);

        auto rightA = area.removeFromLeft(limMeterW);
        rightMeterRect = rightA.withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);
    }

    // ceiling
    auto cA = area.removeFromLeft(int(area.getWidth() * 0.25f));
    ceilingLabel.setBounds(cA.removeFromTop(20));
    ceilingSlider.setBounds(cA);

    // PRE/POST
    constexpr int meterW = 30;
    constexpr int sliderW = 50;
    constexpr int gap = 40;
    constexpr int labelH = 20;

    int totalW = (meterW + sliderW) * 2 + gap;
    auto gainArea = area.removeFromRight(totalW);

    {
        auto preFull = gainArea.removeFromLeft(meterW + sliderW);
        auto labelRect = preFull.removeFromTop(labelH);
        preGainLabel.setBounds(labelRect);
        auto sliderRect = preFull.removeFromRight(sliderW);
        preGainSlider.setBounds({ sliderRect.getX(),
                                  sliderRect.getY(),
                                  sliderW,
                                  sliderRect.getHeight() });
        auto meterArea = preFull.withHeight(sliderRect.getHeight())
            .withY(sliderRect.getY());
        int halfW = meterW / 2;
        preLeftMeterRect = meterArea.removeFromLeft(halfW)
            .withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);
        preRightMeterRect = meterArea
            .withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);
    }
    // gap
    gainArea.removeFromLeft(gap);
    {
        auto postFull = gainArea.removeFromLeft(meterW + sliderW);
        auto labelRect = postFull.removeFromTop(labelH);
        postGainLabel.setBounds(labelRect);
        auto sliderRect = postFull.removeFromRight(sliderW);
        postGainSlider.setBounds({ sliderRect.getX(),
                                   sliderRect.getY(),
                                   sliderW,
                                   sliderRect.getHeight() });

        auto meterArea = postFull.withHeight(sliderRect.getHeight())
            .withY(sliderRect.getY());
        int halfW = meterW / 2;
        postLeftMeterRect = meterArea.removeFromLeft(halfW)
            .withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);
        postRightMeterRect = meterArea
            .withTrimmedTop(meterVMargin)
            .withTrimmedBottom(meterVMargin);
    }
    constexpr int releaseSliderW = 80;
    auto releaseArea = area.removeFromLeft(releaseSliderW);
    releaseLabel.setBounds(releaseArea.removeFromTop(labelH));
    releaseSlider.setBounds(releaseArea);
}

void MaximizerComponent::timerCallback() {
    double now = juce::Time::getMillisecondCounterHiRes() * 0.001;
    double delta = now - lastTimerTime;
    lastTimerTime = now;
    float instL = audioProcessor.getMaximizerPeakLeft();
    float instR = audioProcessor.getMaximizerPeakRight();
    float preL = audioProcessor.getPreGainPeakLeft();
    float preR = audioProcessor.getPreGainPeakRight();
    float postL = audioProcessor.getPostGainPeakLeft();
    float postR = audioProcessor.getPostGainPeakRight();
    if (instL > outerPeakLeft)
        outerPeakLeft = instL;
    else
        outerPeakLeft = juce::jmax(outerPeakLeft - decayRateDbPerSec * (float)delta, instL);
    if (instL > markerPeakLeft) {
        markerPeakLeft = instL;
        holdTimeLeft = 0.0;
    }
    else {
        holdTimeLeft += delta;
        if (holdTimeLeft >= holdDuration)
            markerPeakLeft = juce::jmax(markerPeakLeft - decayRateDbPerSec * (float)delta, instL);
    }

    if (instR > outerPeakRight)
        outerPeakRight = instR;
    else
        outerPeakRight = juce::jmax(outerPeakRight - decayRateDbPerSec * (float)delta, instR);
    if (instR > markerPeakRight) {
        markerPeakRight = instR;
        holdTimeRight = 0.0;
    }
    else {
        holdTimeRight += delta;
        if (holdTimeRight >= holdDuration)
            markerPeakRight = juce::jmax(markerPeakRight - decayRateDbPerSec * (float)delta, instR);
    }
    if (preL > outerPreLeft)
        outerPreLeft = preL;
    else
        outerPreLeft = juce::jmax(outerPreLeft - decayRateDbPerSec * (float)delta, preL);
    if (preL > markerPreLeft) {
        markerPreLeft = preL;
        holdTimePreLeft = 0.0;
    }
    else {
        holdTimePreLeft += delta;
        if (holdTimePreLeft >= holdDuration)
            markerPreLeft = juce::jmax(markerPreLeft - decayRateDbPerSec * (float)delta, preL);
    }
    if (preR > outerPreRight)
        outerPreRight = preR;
    else
        outerPreRight = juce::jmax(outerPreRight - decayRateDbPerSec * (float)delta, preR);
    if (preR > markerPreRight) {
        markerPreRight = preR;
        holdTimePreRight = 0.0;
    }
    else {
        holdTimePreRight += delta;
        if (holdTimePreRight >= holdDuration)
            markerPreRight = juce::jmax(markerPreRight - decayRateDbPerSec * (float)delta, preR);
    }
    if (postL > outerPostLeft)
        outerPostLeft = postL;
    else
        outerPostLeft = juce::jmax(outerPostLeft - decayRateDbPerSec * (float)delta, postL);
    if (postL > markerPostLeft) {
        markerPostLeft = postL;
        holdTimePostLeft = 0.0;
    }
    else {
        holdTimePostLeft += delta;
        if (holdTimePostLeft >= holdDuration)
            markerPostLeft = juce::jmax(markerPostLeft - decayRateDbPerSec * (float)delta, postL);
    }
    if (postR > outerPostRight)
        outerPostRight = postR;
    else
        outerPostRight = juce::jmax(outerPostRight - decayRateDbPerSec * (float)delta, postR);

    if (postR > markerPostRight) {
        markerPostRight = postR;
        holdTimePostRight = 0.0;
    }
    else {
        holdTimePostRight += delta;
        if (holdTimePostRight >= holdDuration)
            markerPostRight = juce::jmax(markerPostRight - decayRateDbPerSec * (float)delta, postR);
    }
    repaint();
}