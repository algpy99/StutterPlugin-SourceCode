/*
  ==============================================================================

    LFOGenerator.h
    Created: 27 Jun 2023 3:40:37pm
    Author:  goupy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace alex_dsp
{
class LFOGenerator
{
public:

    void prepare(const juce::dsp::ProcessSpec& spec);

    void reset();

    struct Parameters
    {
        float m_frequency = 2;
    };

    void setFrequency(float newFrequency);

    void process();

    void processSample();

    void processSine();

    void processSaw();

    void processSquare();

    float getCurrentLFOValue();

    void setParameter(Parameters parameter, float parameterValue);

    enum class LFOType
    {
        kSine,
        kSaw,
        kSquare
    };

    void setLFOType(LFOType newType);



private:

    float sampleRate;

    juce::SmoothedValue<float> m_frequency;

    float m_time;
    float m_deltaTime;
    float m_GlobalBypass{ false };
    float m_LFOValue;

    LFOType _type = LFOType::kSine;
};

}

