/*
  ==============================================================================

    LFOGeneretor.h
    Created: 27 Jun 2023 2:36:41pm
    Author:  goupy

  ==============================================================================
*/

#pragma once

#ifndef LFOGenerator_h
#define LFOGenerator_h

#include <JuceHeader.h>

class LFOGenerator
{
public:

    void prepare(const juce::dsp::ProcessSpec& spec);

    void reset();

    void initialise(const std::function<float(float)>& function, size_t lookupTableNumPoints = 0);

    float processSample(float newInput);

    enum class ParameterId
    {
        kFrequency,
        kBypass
    };

    enum class WaveType
    {
        kSine,
        kSaw,
        kSquare
    };

    void setParameter(ParameterId parameter, float parameterValue);

    void setWaveType(WaveType newWaveType);

private:

    juce::SmoothedValue<float> m_frequency;
    float sampleRate;

    juce::dsp::Phase<float> phase;

    std::function<float(float)> generator;
    std::unique_ptr<juce::dsp::LookupTableTransform<float>> lookupTable;

    bool m_GlobalBypass{ false };

}; // class

#endif /* LFOGenerator_h */