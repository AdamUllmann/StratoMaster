/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 31 Mar 2025 5:27:16pm
    Author:  Adam

  ==============================================================================
*/
#include "CustomLookAndFeel.h"

GearButton::GearButton() : Button("Settings") {
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void GearButton::paintButton(juce::Graphics& g, bool isMouseOver, bool isButtonDown) {
    auto area = getLocalBounds().toFloat().reduced(2.0f);
    auto radius = 1.0f;
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(area, radius);
    g.setColour(juce::Colours::darkgrey);
    g.drawRoundedRectangle(area, radius, 1.0f);
    auto cx = area.getCentreX();
    auto cy = area.getCentreY();
    float R = juce::jmin(area.getWidth(), area.getHeight()) * 0.2f;
    const int numTeeth = 6;
    const float toothLen = R * 0.6f;
    const float toothWidth = R * 0.5f;
    juce::Path gearPath;
    gearPath.addEllipse(cx - R, cy - R, R * 2.0f, R * 2.0f);
    for (int i = 0; i < numTeeth; ++i) {
        float angle = juce::MathConstants<float>::twoPi * i / (float)numTeeth;
        juce::Path tooth;
        tooth.addRectangle(R, -toothWidth * 0.5f, toothLen, toothWidth);
        tooth.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
        gearPath.addPath(tooth);
    }
    g.setColour(isMouseOver ? juce::Colours::white : juce::Colours::lightgrey);
    g.fillPath(gearPath);
    float innerR = R * 0.5f;
    g.setColour(juce::Colours::black);
    g.fillEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour,
    bool isMouseOverButton, bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    float cornerSize = 6.0f;
    juce::Colour base = isButtonDown ? juce::Colour(120, 20, 20) :
        isMouseOverButton ? juce::Colour(80, 80, 80) :
        backgroundColour;
    juce::Colour edge = juce::Colour(20, 20, 20);
    juce::ColourGradient grad(base, bounds.getX(), bounds.getY(),
        edge, bounds.getBottomLeft().getX(), bounds.getBottomLeft().getY(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, cornerSize);
    if (button.getToggleState()) {
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.5f);
    }
    else {
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
    bool isMouseOverButton, bool isButtonDown)
{
    auto font = juce::Font("Arial Black", 16.0f, juce::Font::bold);
    g.setFont(font);
    juce::Colour textColour = button.getToggleState() ? juce::Colours::red.brighter() :
        isMouseOverButton ? juce::Colours::white :
        juce::Colours::lightgrey;
    g.setColour(textColour);
    auto bounds = button.getLocalBounds().reduced(6);
    g.drawFittedText(button.getButtonText(), bounds, juce::Justification::centred, 1);
}


void CustomLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
    bool isMouseOver, bool isMouseDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    bool isActive = button.isFrontTab();
    juce::Colour base = juce::Colour(25, 25, 25);
    juce::Colour edge = juce::Colour(15, 15, 15);
    if (!isActive)
    {
        juce::DropShadow shadow(juce::Colours::black.withAlpha(0.6f), 6, { 0, 2 });
        shadow.drawForRectangle(g, bounds.toNearestInt());
    }
    juce::Path tabShape;
    float corner = 6.0f;
    tabShape.addRoundedRectangle(bounds, corner);
    juce::ColourGradient grad(base, bounds.getX(), bounds.getY(),
        edge, bounds.getBottomLeft().getX(), bounds.getBottomLeft().getY(), false);
    g.setGradientFill(grad);
    g.fillPath(tabShape);
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.strokePath(tabShape, juce::PathStrokeType(1.0f));
    juce::Font font("Verdana", 12.0f, isActive ? juce::Font::bold : juce::Font::plain);
    g.setFont(font);
    g.setColour(isActive ? juce::Colours::white : juce::Colours::lightgrey);
    g.drawText(button.getButtonText(), bounds.reduced(6), juce::Justification::centred);
}


void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    const auto bounds = label.getLocalBounds().toFloat();
    const bool isSliderLabel = label.getParentComponent() && dynamic_cast<juce::Slider*>(label.getParentComponent());
    const bool isStatusLabel = label.getName() == "StatusLabel";
    if (isSliderLabel || isStatusLabel) {
        juce::Colour background = juce::Colour(20, 20, 25);
        juce::Colour border = juce::Colour(60, 80, 100);
        juce::Colour textc = juce::Colour(150, 180, 200);
        g.setColour(background);
        g.fillRoundedRectangle(bounds.reduced(1.0f), 4.0f);
        g.setColour(border.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 1.3f);
        g.setColour(textc);
        juce::Font font("Consolas", label.getFont().getHeight() * 0.8f, juce::Font::plain);
        g.setFont(font);
        juce::String text = label.getText();
        if (!isStatusLabel) {
            text = text.retainCharacters("0123456789.-").substring(0, 5);
            if (text.endsWithChar('.'))
                text = text.dropLastCharacters(1);
        }
        auto paddedBounds = label.getLocalBounds().reduced(4, 0);
        g.drawText(text, paddedBounds, juce::Justification::centred, false);
        return;
    }
    juce::LookAndFeel_V4::drawLabel(g, label);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float maxRadius = juce::jmin(width, height) * 0.5f;

    const float ringThicknessRatio = 0.2f;
    const float shadowOffsetRatio = 0.1f;
    const float maxOutwardFactor = ringThicknessRatio + shadowOffsetRatio;
    const float knobRadius = maxRadius / (1.0f + maxOutwardFactor);

    const float shadowOffset = knobRadius * shadowOffsetRatio;
    const float ringThickness = knobRadius * ringThicknessRatio;
    const float innerRadius = knobRadius - ringThickness;

    const float pointerThickness = innerRadius * 0.2f;
    const float pointerLength = innerRadius * 0.6f;

    const float highlightRadius = innerRadius * 0.15f;

    // drop shadow
    {
        float shadowScale = 1.25f;
        const int shadowDiameter = static_cast<int>(knobRadius * 2.0f * shadowScale);

        static juce::Image noiseShadowImage;
        static int lastShadowDiameter = 0;

        if (noiseShadowImage.isNull() || lastShadowDiameter != shadowDiameter) {
            noiseShadowImage = juce::Image(juce::Image::ARGB, shadowDiameter, shadowDiameter, true);
            lastShadowDiameter = shadowDiameter;
            juce::Random random;

            for (int y = 0; y < shadowDiameter; ++y) {
                for (int x = 0; x < shadowDiameter; ++x) {
                    float noise = random.nextFloat();

                    float dx = (x - shadowDiameter * 0.5f) / (shadowDiameter * 0.5f);
                    float dy = (y - shadowDiameter * 0.5f) / (shadowDiameter * 0.5f);
                    float dist = std::sqrt(dx * dx + dy * dy);
                    float fade = juce::jlimit(0.0f, 1.0f, 1.0f - dist);

                    float alpha = (0.7f + noise * 0.3f) * fade;
                    noiseShadowImage.setPixelAt(x, y, juce::Colour::fromRGBA(0, 0, 0, static_cast<juce::uint8>(alpha * 255)));
                }
            }
        }
        float extra = (shadowDiameter - knobRadius * 2.0f) / 2.0f;
        juce::Path shadowPath;
        shadowPath.addEllipse(centreX - knobRadius + shadowOffset - extra,
            centreY - knobRadius + shadowOffset - extra,
            knobRadius * 2.0f + extra * 2.0f,
            knobRadius * 2.0f + extra * 2.0f);

        g.saveState();
        g.reduceClipRegion(shadowPath);
        g.drawImage(noiseShadowImage,
            centreX - knobRadius + shadowOffset - extra,
            centreY - knobRadius + shadowOffset - extra,
            knobRadius * 2.0f + extra * 2.0f,
            knobRadius * 2.0f + extra * 2.0f,
            0, 0,
            shadowDiameter,
            shadowDiameter);
        g.restoreState();
    }

    // metallic outer ring
    {
        juce::Path ringPath;
        ringPath.addEllipse(centreX - knobRadius,
            centreY - knobRadius,
            knobRadius * 2.0f,
            knobRadius * 2.0f);

        juce::PathStrokeType ringStroke(ringThickness);
        juce::Path strokedRing;
        ringStroke.createStrokedPath(strokedRing, ringPath);

        juce::ColourGradient ringGradient(
            juce::Colour(170, 170, 170), //  top-left
            centreX - knobRadius, centreY - knobRadius,
            juce::Colour(60, 60, 60),    //  bottom-right
            centreX + knobRadius, centreY + knobRadius,
            false
        );
        ringGradient.addColour(0.5, juce::Colour(110, 110, 110));

        g.setGradientFill(ringGradient);
        g.fillPath(strokedRing);
    }

    // knob face
    {
        juce::Rectangle<float> knobBounds(centreX - innerRadius,
            centreY - innerRadius,
            innerRadius * 2.0f,
            innerRadius * 2.0f);
        juce::ColourGradient faceGradient(
            juce::Colour(80, 80, 80),         // center
            centreX, centreY,
            juce::Colour(40, 40, 40),       // edge
            centreX + innerRadius, centreY,
            true                          // radial
        );
        faceGradient.addColour(0.2, juce::Colour(70, 70, 70));
        faceGradient.addColour(0.4, juce::Colour(60, 60, 60));
        faceGradient.addColour(0.6, juce::Colour(50, 50, 50));
        faceGradient.addColour(0.8, juce::Colour(60, 60, 60));
        g.setGradientFill(faceGradient);
        g.fillEllipse(knobBounds);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawEllipse(knobBounds, 1.0f);
    }
    // circular highlight
    {
        g.saveState();
        {
            juce::Path faceClip;
            faceClip.addEllipse(centreX - innerRadius,
                centreY - innerRadius,
                innerRadius * 2.0f,
                innerRadius * 2.0f);
            g.reduceClipRegion(faceClip);
            float highlightX = centreX - (innerRadius * 0.4f);
            float highlightY = centreY - (innerRadius * 0.5f);
            juce::Path highlight;
            highlight.addEllipse(highlightX, highlightY,
                highlightRadius * 2.0f,
                highlightRadius * 2.0f);
            juce::ColourGradient highlightGrad(
                juce::Colours::white.withAlpha(0.6f),
                highlightX + highlightRadius,
                highlightY + highlightRadius,
                juce::Colours::white.withAlpha(0.0f),
                highlightX + highlightRadius,
                highlightY + highlightRadius,
                true
            );
            g.setGradientFill(highlightGrad);
            g.fillPath(highlight);
        }
        g.restoreState();
    }
    // pointer (indicator)
    {
        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        juce::Path pointer;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f,
            -innerRadius,
            pointerThickness,
            pointerLength,
            1.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle)
            .translated(centreX, centreY));

        juce::Colour pointerColour = slider.findColour(juce::Slider::rotarySliderFillColourId);
        g.setColour(pointerColour);
        g.fillPath(pointer);
    }
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider) {
    const float trackThickness = 10.0f;
    if (style == juce::Slider::LinearHorizontal) {
        const float trackY = y + (height - trackThickness) * 0.5f;
        juce::Rectangle<float> trackBounds(x, trackY, width, trackThickness);
        juce::Path trackPath;
        trackPath.addRoundedRectangle(trackBounds.getX(), trackBounds.getY(),
            trackBounds.getWidth(), trackBounds.getHeight(), 5.0f);
        juce::ColourGradient trackGradient(juce::Colour(170, 170, 170),
            trackBounds.getX(), trackBounds.getY(),
            juce::Colour(140, 140, 140),
            trackBounds.getRight(), trackBounds.getBottom(), false);
        trackGradient.addColour(0.5, juce::Colour(110, 110, 110));
        g.setGradientFill(trackGradient);
        g.fillPath(trackPath);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(trackPath, juce::PathStrokeType(1.0f));
        juce::Rectangle<float> innerTrack = trackBounds.reduced(2);
        g.setColour(juce::Colour(10, 10, 10));
        g.fillRoundedRectangle(innerTrack.getX(), innerTrack.getY(),
            innerTrack.getWidth(), innerTrack.getHeight(), 4.0f);
        {
            const int numLEDs = 29;
            float trackLeft = trackBounds.getX();
            float trackWidth = trackBounds.getWidth();
            float centerY = trackBounds.getCentreY();
            float ledRadius = 2.0f;
            juce::Colour ledOnColour = slider.findColour(juce::Slider::rotarySliderFillColourId);
            juce::Colour ledOffColour = juce::Colour(30, 30, 30);
            float thumbCenterX = sliderPos;
            for (int i = 0; i < numLEDs; ++i) {
                float ledFraction = (i + 1.0f) / (numLEDs + 1.0f);
                float cx = trackLeft + ledFraction * trackWidth;
                float diff = thumbCenterX - cx;
                float intensity = juce::jlimit(0.0f, 1.0f, diff / (ledRadius * 2.0f));
                juce::Colour baseColour = ledOffColour.interpolatedWith(ledOnColour, intensity);
                juce::Rectangle<float> ledRect(cx - ledRadius, centerY - ledRadius,
                    ledRadius * 2.0f, ledRadius * 2.0f);
                g.setColour(baseColour);
                g.fillEllipse(ledRect);
                g.setColour(juce::Colours::white.withAlpha(0.4f * intensity));
                g.fillEllipse(ledRect.reduced(ledRadius * 0.3f));
            }
        }
        const float thumbDiameter = height * 0.8f;
        const float thumbX = sliderPos - thumbDiameter * 0.5f;
        const float thumbY = y + (height - thumbDiameter) * 0.5f;
        juce::Rectangle<float> thumbBounds(thumbX, thumbY, thumbDiameter, thumbDiameter);
        juce::Path shadowPath;
        shadowPath.addEllipse(thumbBounds.translated(2.0f, 2.0f));
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillPath(shadowPath);
        juce::Path thumbPath;
        thumbPath.addEllipse(thumbBounds);
        juce::ColourGradient radialGrad(juce::Colour(220, 220, 220),
            thumbBounds.getCentreX(), thumbBounds.getCentreY(),
            juce::Colour(100, 100, 100),
            thumbBounds.getRight(), thumbBounds.getBottom(),
            true);
        radialGrad.addColour(0.5, juce::Colour(160, 160, 160));
        g.setGradientFill(radialGrad);
        g.fillPath(thumbPath);
        juce::Path highlight;
        float highlightDiameter = thumbDiameter * 0.4f;
        highlight.addEllipse(thumbX + thumbDiameter * 0.15f,
            thumbY + thumbDiameter * 0.15f,
            highlightDiameter, highlightDiameter);
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillPath(highlight);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(thumbPath, juce::PathStrokeType(1.0f));
        {
            float centerLedRadius = 3.0f;
            juce::Point<float> center = thumbBounds.getCentre();
            juce::Rectangle<float> centerLedRect(center.x - centerLedRadius, center.y - centerLedRadius, centerLedRadius * 2.0f, centerLedRadius * 2.0f);
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            g.fillEllipse(centerLedRect);
        }
    }
    else if (style == juce::Slider::LinearVertical) {
        const float trackWidth = 10.0f;
        const float trackX = x + (width - trackWidth) * 0.5f;
        juce::Rectangle<float> trackBounds(trackX, y, trackWidth, height);
        juce::Path trackPath;
        trackPath.addRoundedRectangle(trackBounds.getX(), trackBounds.getY(),
            trackBounds.getWidth(), trackBounds.getHeight(), 5.0f);
        juce::ColourGradient trackGradient(juce::Colour(170, 170, 170),
            trackBounds.getX(), trackBounds.getY(),
            juce::Colour(100, 100, 100),
            trackBounds.getRight(), trackBounds.getBottom(), false);
        trackGradient.addColour(0.5, juce::Colour(110, 110, 110));
        g.setGradientFill(trackGradient);
        g.fillPath(trackPath);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(trackPath, juce::PathStrokeType(1.0f));
        juce::Rectangle<float> innerTrack = trackBounds.reduced(2);
        g.setColour(juce::Colour(10, 10, 10));
        g.fillRoundedRectangle(innerTrack.getX(), innerTrack.getY(),
            innerTrack.getWidth(), innerTrack.getHeight(), 4.0f);
        {
            const int numLEDs = 29;
            float trackBottom = trackBounds.getBottom();
            float trackHeight = trackBounds.getHeight();
            float centerX = trackBounds.getCentreX();
            float ledRadius = 2.0f;
            juce::Colour ledOnColour = slider.findColour(juce::Slider::trackColourId);
            juce::Colour ledOffColour = juce::Colour(30, 30, 30);
            float thumbCenterY = sliderPos;
            for (int i = 0; i < numLEDs; ++i) {
                float ledFraction = (i + 1.0f) / (numLEDs + 1.0f);
                float cy = trackBottom - ledFraction * trackHeight;
                float diff = cy - thumbCenterY;
                float intensity = juce::jlimit(0.0f, 1.0f, diff / (ledRadius * 2.0f));
                juce::Colour baseColour = ledOffColour.interpolatedWith(ledOnColour, intensity);
                juce::Rectangle<float> ledRect(centerX - ledRadius, cy - ledRadius,
                    ledRadius * 2.0f, ledRadius * 2.0f);
                g.setColour(baseColour);
                g.fillEllipse(ledRect);
                g.setColour(juce::Colours::white.withAlpha(0.4f * intensity));
                g.fillEllipse(ledRect.reduced(ledRadius * 0.3f));
            }
        }
        const float thumbDiameter = height * 0.08f;
        const float thumbY = sliderPos - (thumbDiameter * 0.5f);
        const float thumbX = x + (width - thumbDiameter) * 0.5f;
        juce::Rectangle<float> thumbBounds(thumbX, thumbY, thumbDiameter, thumbDiameter);
        juce::Path shadowPath;
        shadowPath.addEllipse(thumbBounds.translated(2.0f, 2.0f));
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillPath(shadowPath);
        juce::Path thumbPath;
        thumbPath.addEllipse(thumbBounds);
        juce::ColourGradient radialGrad(juce::Colour(220, 220, 220),
            thumbBounds.getCentreX(), thumbBounds.getCentreY(),
            juce::Colour(100, 100, 100),
            thumbBounds.getRight(), thumbBounds.getBottom(),
            true);
        radialGrad.addColour(0.5, juce::Colour(160, 160, 160));
        g.setGradientFill(radialGrad);
        g.fillPath(thumbPath);
        juce::Path highlight;
        float highlightDiameter = thumbDiameter * 0.4f;
        highlight.addEllipse(thumbX + thumbDiameter * 0.15f,
            thumbY + thumbDiameter * 0.15f,
            highlightDiameter, highlightDiameter);
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillPath(highlight);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.strokePath(thumbPath, juce::PathStrokeType(1.0f));
        {
            float centerLedRadius = 2.0f;
            juce::Point<float> center = thumbBounds.getCentre();
            juce::Rectangle<float> centerLedRect(center.x - centerLedRadius, center.y - centerLedRadius, centerLedRadius * 2.0f, centerLedRadius * 2.0f);
            g.setColour(slider.findColour(juce::Slider::trackColourId));
            g.fillEllipse(centerLedRect);
        }
    }
}