# Stratomaster

![Stratomaster](https://github.com/AdamUllmann/images-for-repos/blob/main/stratomaster.png?raw=true)

**Stratomaster** is an all-in-one audio mastering plugin that includes:
- **8-Band Parametric (and Shelving) EQ**
- **Multiband Compressor**
- **Stereo Imager**
- **Maximizer (Limiter)**
- **Auto Mastering / Auto EQ** Feature

---

## Table of Contents
1. [Overview](#overview)
2. [8-Band EQ](#8-band-eq)
3. [Multiband Compressor](#multiband-compressor)
4. [Stereo Imager](#stereo-imager)
5. [Maximizer](#maximizer)
6. [Auto EQ / Auto Mastering](#auto-eq--auto-mastering)
7. [Additional Notes](#additional-notes)

---

## Overview

Stratomaster is a single plugin designed to handle multiple stages of mixing or mastering:
- **EQ** for tone shaping (parametric, low/high shelf, plus optional low-pass and high-pass filters).
- **Multiband compression** (in progress), for dynamic control of separate frequency bands.
- **Stereo imaging** to adjust stereo width.
- **Maximizer** for final loudness and peak limiting.
- **Auto Mastering** logic that can analyze the incoming frequency content and automatically adjust each EQ band (the “Auto EQ” process).

All parameters are controlled via a single `AudioProcessorValueTreeState`, so changes persist and can be automated.

---

## 8-Band EQ

### Features
- **8 Separate Bands**  
  Each band has:
  - **Frequency** parameter (`BandXFreq`)
  - **Gain** parameter (`BandXGain`)
  - **Q** (quality) parameter (`BandXQ`)
  - **Filter Type** (`BandXFilterType`): choose from Low Pass, Peak (Parametric), High Pass, Low Shelf, High Shelf.
- **Drag-and-Drop Curve** (in the `ParametricEQComponent`)  
  – You can grab each band’s “handle” in the EQ display to shift frequency/gain visually.
- **Real-Time Spectrum** overlay.

---

## Multiband Compressor

*(Currently in progress, conceptual code included.)*

### Features
- **Multiple Frequency Bands** (e.g., Low, Mid, High or more).
- Each band has its own **threshold, ratio, attack, release,** and optional **makeup** gain controls.
- **Crossover Frequencies** let you define the boundary between bands.

### How It Works
1. **Split** the incoming audio into separate bands via Linkwitz-Riley crossover filters.
2. Each band gets processed by an individual compressor instance with user-defined parameters.
3. The bands are then **summed back** together for output, ensuring a cohesive final sound.

---

## Stereo Imager

### Features
- **Width Control** (`ImagerWidth`)  
  Adjusts the stereo field’s mid/side balance.  
  - **Values < 1** reduce stereo width (narrower image).  
  - **Values > 1** expand the stereo width.
- **Real-Time Scope** display  
  A circular “Lissajous” style scope that visualizes stereo correlation and phase relationships.

---

## Maximizer

### Features
- **Threshold** (`MaxThreshold`): The point at which the maximizer begins limiting.
- **Ceiling** (`MaxCeiling`): The maximum level allowed at the output (e.g., -0.1 dB or 0.0 dB).
- **Release** (`MaxRelease`): Controls how quickly the limiter stops attenuating after the signal falls below the threshold.

### Typical Usage
1. **Set** the **Threshold** to determine how aggressively to limit. Lower values (like -10 dB) create more limiting and potentially higher overall loudness.
2. **Adjust** the **Ceiling** to your target output level (e.g., -1 dB True Peak).
3. **Tune** the **Release** to avoid pumping; shorter releases can cause distortion, while longer ones may reduce perceived loudness.

---

## Auto EQ / Auto Mastering

### Features
- **AutoEQ** toggles an internal analysis of the frequency spectrum.
- It **averages** the energy in separate frequency bands and attempts to match them to a **target** or “globalMeanDb.”
- Over time, it **auto-adjusts** each band’s gain to flatten or balance the overall frequency response.
- This ensures a "balanced" frequency response, which may or may not be desired by professional audio engineers, so a future update will allow the user to costomize their desired frequency response target.

### How It Works
1. Every block of audio is **FFT-analyzed**.
2. For each band, the plugin measures the average magnitude.
3. If a band is *too loud* relative to the global average, the plugin gradually **cuts** it; if *too soft*, the plugin gradually **boosts** it.
4. Once it deems the response “stable” (the difference is small), it stops the auto-adjustment.

### Typical Usage
1. Click “**Auto EQ**” or “Auto Mastering” in the UI (depending on your final design) to enable.
2. Play audio (ideally representative of the track), allowing the plugin to analyze each band.
3. Watch as each band’s gain is slowly adjusted. When it stabilizes, the process stops, offering a **quick** broad-strokes EQ starting point.

---

## Installation for VST3
1. Download the plugin from the latest release (you want the .vst3 file).
2. Move the downloaded .vst3 file to C:Program_Files/Common_files/VST3.
3. Follow your DAW's instructions for adding and scanning new plugins.
  - In FL Studio, you would go to options -> manage plugins -> "find installed plugins"
4. Locate Stratomaster in your DAW's plugin list and add it to a mixer track.

---

## Installation for Standalone on Windows
1. Download the plugin from the latest release (you want the .exe file).
2. Run Stratomaster.exe
3. You are done. It's as simple as that.

## Additional Notes

- **Parameter Sync**: All parameters (EQ band freq/gain/Q, compressor thresholds, stereo width, etc.) are stored in a `ValueTreeState`, so they save/load with your session.
- **Graphics / UI**: Each section (EQ, Compressor, Imager, Maximizer) has a dedicated UI component.

Stratomaster aims to simplify many steps of the mastering chain into one plugin, letting you quickly tweak EQ curves, multiband dynamics, stereo field, and final limiting. Meanwhile, the **Auto** feature can provide a good starting point for novices or speed up advanced workflows.

