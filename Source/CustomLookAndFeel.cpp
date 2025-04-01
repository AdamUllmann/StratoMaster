/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 31 Mar 2025 5:27:16pm
    Author:  Adam

  ==============================================================================
*/
#include "CustomLookAndFeel.h"

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