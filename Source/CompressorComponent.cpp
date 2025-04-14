/*
  ==============================================================================

    CompressorComponent.cpp
    Created: 3 Feb 2025 4:05:11am
    Author:  Adam

  ==============================================================================
*/

#include "CompressorComponent.h"

namespace {
    const juce::Colour lowColour = juce::Colour::fromRGB(200, 100, 40);   // red
    const juce::Colour midColour = juce::Colour::fromRGB(190, 150, 20);   // yellow
    const juce::Colour highColour = juce::Colour::fromRGB(70, 70, 200);   // blue
}

//==============================================================================
CompressorComponent::CompressorComponent(juce::AudioProcessorValueTreeState& apvts) : apvtsRef(apvts)
{
    addBandControls(lowControls, lowAttach, "MBCompLow", lowColour);
    addBandControls(midControls, midAttach, "MBCompMid", midColour);
    addBandControls(highControls, highAttach, "MBCompHigh", highColour);
}

CompressorComponent::~CompressorComponent()
{

}

void CompressorComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillAll();

    paintBandBox(g, lowArea);
    paintBandBox(g, midArea);
    paintBandBox(g, highArea);

    g.setColour(juce::Colours::darkgrey.darker());
    auto area = getLocalBounds().reduced(10);
    int rowHeight = area.getHeight() / 3;
    int line1Y = area.getY() + rowHeight;
    g.drawLine((float)area.getX(), (float)line1Y, (float)area.getRight(), (float)line1Y, 2.0f);

    int line2Y = area.getY() + rowHeight * 2;
    g.drawLine((float)area.getX(), (float)line2Y, (float)area.getRight(), (float)line2Y, 2.0f);
}

void CompressorComponent::resized() {
    auto fullArea = getLocalBounds().reduced(10);
    int bandHeight = fullArea.getHeight() / 3;
    lowArea = fullArea.removeFromTop(bandHeight);
    midArea = fullArea.removeFromTop(bandHeight);
    highArea = fullArea;
    layoutBand(lowControls, lowArea);
    layoutBand(midControls, midArea);
    layoutBand(highControls, highArea);
}

void CompressorComponent::addBandControls(BandControls& bc, CompressorAttachments& attach, const juce::String& prefix, juce::Colour colour) {
    // threshold
    bc.threshold.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bc.threshold.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bc.threshold.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(bc.threshold);
    auto threshID = prefix + juce::String("Threshold");
    attach.thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, threshID, bc.threshold));
    bc.thresholdLabel.setText("Threshold", juce::dontSendNotification);
    bc.thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bc.thresholdLabel);

    // ratio
    bc.ratio.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bc.ratio.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bc.ratio.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(bc.ratio);
    auto ratioID = prefix + juce::String("Ratio");
    attach.ratioAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, ratioID, bc.ratio));
    bc.ratioLabel.setText("Ratio", juce::dontSendNotification);
    bc.ratioLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bc.ratioLabel);

    // attack
    bc.attack.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bc.attack.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bc.attack.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(bc.attack);
    auto attackID = prefix + juce::String("Attack");
    attach.attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, attackID, bc.attack));
    bc.attackLabel.setText("Attack", juce::dontSendNotification);
    bc.attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bc.attackLabel);

    // release
    bc.release.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bc.release.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bc.release.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(bc.release);
    auto releaseID = prefix + juce::String("Release");
    attach.releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, releaseID, bc.release));
    bc.releaseLabel.setText("Release", juce::dontSendNotification);
    bc.releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bc.releaseLabel);

    // makeup
    bc.makeup.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bc.makeup.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bc.makeup.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(bc.makeup);
    auto makeupID = prefix + juce::String("Makeup");
    attach.makeupAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(apvtsRef, makeupID, bc.makeup));
    bc.makeupLabel.setText("Makeup", juce::dontSendNotification);
    bc.makeupLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bc.makeupLabel);
}

void CompressorComponent::layoutBand(BandControls& band, juce::Rectangle<int> area) {
    auto totalWidth = area.getWidth();
    auto totalHeight = area.getHeight();
    int labelHeight = 20;
    int knobDiameter = 60;
    int columns = 5;
    int colWidth = totalWidth / columns;
    auto currentArea = area;
    {
        auto knobArea = currentArea.removeFromLeft(colWidth);
        band.thresholdLabel.setBounds(knobArea.removeFromTop(labelHeight));
        band.threshold.setBounds(knobArea.withSizeKeepingCentre(knobDiameter, knobDiameter));
    }
    {
        auto knobArea = currentArea.removeFromLeft(colWidth);
        band.ratioLabel.setBounds(knobArea.removeFromTop(labelHeight));
        band.ratio.setBounds(knobArea.withSizeKeepingCentre(knobDiameter, knobDiameter));
    }
    {
        auto knobArea = currentArea.removeFromLeft(colWidth);
        band.attackLabel.setBounds(knobArea.removeFromTop(labelHeight));
        band.attack.setBounds(knobArea.withSizeKeepingCentre(knobDiameter, knobDiameter));
    }
    {
        auto knobArea = currentArea.removeFromLeft(colWidth);
        band.releaseLabel.setBounds(knobArea.removeFromTop(labelHeight));
        band.release.setBounds(knobArea.withSizeKeepingCentre(knobDiameter, knobDiameter));
    }
    {
        auto knobArea = currentArea;
        band.makeupLabel.setBounds(knobArea.removeFromTop(labelHeight));
        band.makeup.setBounds(knobArea.withSizeKeepingCentre(knobDiameter, knobDiameter));
    }
}

void CompressorComponent::paintBandBox(juce::Graphics& g, juce::Rectangle<int> bandArea) const {
    auto top = (float)bandArea.getY();
    auto bottom = (float)bandArea.getBottom();
    auto left = (float)bandArea.getX();
    auto right = (float)bandArea.getRight();
    juce::ColourGradient gradient(juce::Colour(40, 40, 40), 0, 0, juce::Colour(40, 40, 40), getWidth(), 0, false);
    gradient.addColour(0.2, juce::Colour(60, 60, 60));
    gradient.addColour(0.5, juce::Colour(80, 80, 80));
    gradient.addColour(0.8, juce::Colour(60, 60, 60));
    g.setGradientFill(gradient);
    g.fillRect(bandArea);
    g.setColour(juce::Colours::black);
    g.drawRect(bandArea, 2.0f);
}