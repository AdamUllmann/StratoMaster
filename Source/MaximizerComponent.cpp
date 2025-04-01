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

void MaximizerComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();

    // background for the meter
    g.setColour(juce::Colours::black);
    g.fillRect(meterRect);
    float peakDb = audioProcessor.getMaximizerPeak();
    float peakFill = dBToFraction(peakDb);
    float yStartFrac = dBToFraction(yellowStartDb);
    float rStartFrac = dBToFraction(redStartDb);
    int meterHeight = meterRect.getHeight();
    int meterBottom = meterRect.getBottom();

    // ============== GREEN FILL ==============
    float greenTopFrac = std::min(peakFill, yStartFrac);
    int greenPixels = (int)(meterHeight * greenTopFrac);
    int greenY = meterBottom - greenPixels;
    g.setColour(juce::Colours::limegreen);
    g.fillRect(meterRect.getX(), greenY, meterRect.getWidth(), greenPixels);

    // ============== YELLOW FILL ==============
    if (peakFill > yStartFrac)
    {
        float yLowFrac = yStartFrac;
        float yHighFrac = std::min(peakFill, rStartFrac);
        int yLowPix = (int)(meterHeight * yLowFrac);
        int yHighPix = (int)(meterHeight * yHighFrac);
        int fillHeight = yHighPix - yLowPix;
        int fillY = meterBottom - yHighPix;
        g.setColour(juce::Colours::yellow);
        g.fillRect(meterRect.getX(), fillY, meterRect.getWidth(), fillHeight);
    }

    // ============== RED FILL ===============
    if (peakFill > rStartFrac)
    {
        float rLowFrac = rStartFrac;
        float rHighFrac = peakFill;
        int rLowPix = (int)(meterHeight * rLowFrac);
        int rHighPix = (int)(meterHeight * rHighFrac);
        int fillHeight = rHighPix - rLowPix;
        int fillY = meterBottom - rHighPix;

        g.setColour(juce::Colours::red);
        g.fillRect(meterRect.getX(), fillY, meterRect.getWidth(), fillHeight);
    }
    for (float dBtick = 0.0f; dBtick >= -60.0f; dBtick -= 6.0f)
    {
        float frac = dBToFraction(dBtick);
        int yPix = meterBottom - (int)(meterHeight * frac);
        g.setColour(juce::Colours::grey);
        g.drawLine((float)meterRect.getX(),
            (float)yPix,
            (float)(meterRect.getRight()),
            (float)yPix,
            1.0f);
        juce::String labelText = juce::String((int)dBtick) + " dB";
        g.setColour(juce::Colours::white);
        g.drawFittedText(labelText,
            meterRect.getX() - 35,
            yPix - 7,
            30, 14,
            juce::Justification::centredRight,
            1);
    }
}

void MaximizerComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    int meterWidth = 50;
    float sliderWidth = 0.02f;
    auto thresholdArea = area.removeFromLeft((int)(area.getWidth() * 0.2f));
    thresholdLabel.setBounds(thresholdArea.removeFromTop(20));
    thresholdSlider.setBounds(thresholdArea);
    auto meterArea = area.removeFromLeft(meterWidth);
    meterRect = meterArea;
    auto ceilingArea = area.removeFromLeft((int)(area.getWidth() * 0.25f));
    ceilingLabel.setBounds(ceilingArea.removeFromTop(20));
    ceilingSlider.setBounds(ceilingArea);
    auto releaseArea = area;
    releaseLabel.setBounds(releaseArea.removeFromTop(20));
    releaseSlider.setBounds(releaseArea.withSizeKeepingCentre(250, 250));
}

void MaximizerComponent::timerCallback()
{
    repaint();
}