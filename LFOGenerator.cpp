/*
  ==============================================================================

    LFOGenerator.cpp
    Created: 27 Jun 2023 3:40:37pm
    Author:  goupy

  ==============================================================================
*/

#include "LFOGenerator.h"

void alex_dsp::LFOGenerator::prepare(const juce::dsp::ProcessSpec &spec)
{
    m_frequency = 1;
    m_time = 0.0;
    sampleRate = spec.sampleRate;
    m_deltaTime = 1 / sampleRate;
}

void alex_dsp::LFOGenerator::process()
{    
    if (m_GlobalBypass)
    {
        m_LFOValue = 0.0;
        return;
    }

    
    if (m_time >= std::numeric_limits<float>::max())
    {
        m_time = 0.0;
    }
    
    //m_LFOValue = sin(2 * juce::double_Pi * m_frequency * m_time);

    float phase = (m_time * m_frequency) - floor(m_time * m_frequency);
    m_LFOValue = (2.0f * 10) * (phase < 0.5f ? phase : (1.0f - phase)) - 10;

    m_time += m_deltaTime;
}

float alex_dsp::LFOGenerator::getCurrentLFOValue()
{
    return m_LFOValue;
}

void alex_dsp::LFOGenerator::setParameter(ParameterId parameter, float parameterValue)
{
    switch (parameter)
    {
    case alex_dsp::LFOGenerator::ParameterId::kFrequency: m_frequency = static_cast<int>(parameterValue); break;
    case alex_dsp::LFOGenerator::ParameterId::kBypass: m_GlobalBypass = static_cast<bool>(parameterValue); break;
    }
}