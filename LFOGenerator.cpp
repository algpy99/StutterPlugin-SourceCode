/*
  ==============================================================================

    LFOGeneretor.cpp
    Created: 27 Jun 2023 2:36:41pm
    Author:  goupy

  ==============================================================================
*/

#include "LFOGenerator.h"

void LFOGenerator::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    m_frequency.reset(sampleRate, 0.05);
    m_frequency.setTargetValue(1.0);

    reset();
}

void LFOGenerator::reset()
{
    phase.reset();
}

void LFOGenerator::initialise(const std::function<float(float)>& function,
    size_t lookupTableNumPoints)
{
    if (lookupTableNumPoints != 0)
    {
        auto* table = new juce::dsp::LookupTableTransform<float>(function,
            -juce::MathConstants<float>::pi,
            juce::MathConstants<float>::pi,
            lookupTableNumPoints);

        lookupTable.reset(table);
        generator = [table](float x) { return (*table) (x); };
    }

    else
    {
        generator = function;
    }
}

float LFOGenerator::processSample(float newInput)
{
    auto increment = juce::MathConstants<float>::twoPi * m_frequency.getNextValue() / sampleRate;
    return newInput + generator(phase.advance(increment) - juce::MathConstants<float>::pi);
}

void LFOGenerator::setParameter(ParameterId parameter, float parameterValue)
{
    switch (parameter)
    {
    case LFOGenerator::ParameterId::kFrequency: m_frequency = static_cast<int>(parameterValue); break;
    case LFOGenerator::ParameterId::kBypass: m_GlobalBypass = static_cast<bool>(parameterValue); break;
    }
}

void LFOGenerator::setWaveType(WaveType newWaveType)
{
    switch (newWaveType)
    {
        case LFOGenerator::WaveType::kSine:
        {
            initialise([](float x) {return std::sin(x); });
            break;
        }

        case LFOGenerator::WaveType::kSaw:
        {
            initialise([](float x) {return x / juce::MathConstants<float>::pi; });
            break;
        }

        case LFOGenerator::WaveType::kSquare:
        {
            initialise([](float x) {return x < 0.0f ? -1.0f : 1.0f; });
            break;
        }
    }
}
