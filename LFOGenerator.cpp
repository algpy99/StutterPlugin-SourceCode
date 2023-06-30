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
    m_time = 0.0;
    sampleRate = spec.sampleRate;
    m_deltaTime = 1 / sampleRate;
    reset();
}

void alex_dsp::LFOGenerator::reset()
{
    if (sampleRate <= 0) return;

    m_frequency.reset(sampleRate, 0.05);
    m_frequency.setTargetValue(1.0);
}

void alex_dsp::LFOGenerator::setFrequency(float newFrequency)
{
    m_frequency.setTargetValue(newFrequency);
    DBG("frequency is: " << newFrequency);
}

void alex_dsp::LFOGenerator::setLFOType(alex_dsp::LFOGenerator::LFOType newType)
{
    switch (newType)
    {
    case LFOType::kSine:
    {
        _type = newType;
        break;
    }
    case LFOType::kSaw:
    {
        _type = newType;
        break;
    }
    case LFOType::kSquare:
    {
        _type = newType;
        break;
    }
    }
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

    processSample();

    //float phase = (m_time * m_frequency) - floor(m_time * m_frequency);
    //m_LFOValue = (2.0f * 10) * (phase < 0.5f ? phase : (1.0f - phase)) - 10;

    //m_time += m_deltaTime;
}

void alex_dsp::LFOGenerator::processSample()
{
    switch (_type)
    {
    case LFOType::kSine:
    {
        return processSine();
        break;
    }
    case LFOType::kSaw:
    {
        return processSaw();
        break;
    }
    case LFOType::kSquare:
    {
        return processSquare();
        break;
    }
    }
}

void alex_dsp::LFOGenerator::processSine()
{
    m_LFOValue = sin(2 * juce::double_Pi * m_frequency.getNextValue() * m_time);
    m_time += m_deltaTime;
}

void alex_dsp::LFOGenerator::processSaw()
{
    float phase = (m_time * m_frequency.getNextValue()) - floor(m_time * m_frequency.getNextValue());
    m_LFOValue = (2.0f * 1) * (phase < 0.5f ? phase : (1.0f - phase)) - 1;
    m_time += m_deltaTime;
}

void alex_dsp::LFOGenerator::processSquare()
{
    
}

float alex_dsp::LFOGenerator::getCurrentLFOValue()
{
    return m_LFOValue;
}

/*
void alex_dsp::LFOGenerator::setParameter(Parameters parameter, float parameterValue)
{
    switch (parameter)
    {
    case alex_dsp::LFOGenerator::Parameters::kFrequency: m_frequency = static_cast<int>(parameterValue); break;
    case alex_dsp::LFOGenerator::Parameters::kBypass: m_GlobalBypass = static_cast<bool>(parameterValue); break;
    }
}
*/