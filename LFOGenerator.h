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

    enum class ParameterId
    {
        kFrequency,
        kBypass,
    };

    void LFOGenerator::process();

    float LFOGenerator::getCurrentLFOValue();

    void LFOGenerator::setParameter(ParameterId parameter, float parameterValue);




private:

    float sampleRate;

    float m_frequency;
    float m_time;
    float m_deltaTime;
    float m_GlobalBypass{ false };
    float m_LFOValue;
};
}

