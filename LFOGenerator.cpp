/*
  ==============================================================================

    LFOGenerator.cpp
    Created: 27 Jun 2023 3:40:37pm
    Author:  goupy

  ==============================================================================
*/

#include "LFOGenerator.h"

template <typename SampleType>
LFOGenerator<SampleType>::LFOGenerator()
{

}

template <typename SampleType>
void LFOGenerator<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    _sampleRate = spec.sampleRate;
    reset();
}

template <typename SampleType>
void LFOGenerator<SampleType>::reset()
{
    if (_sampleRate <= 0) return;

    _frequency.reset(_sampleRate, 0.02);
    _frequency.setTargetValue(0.0);

    _mix.reset(_sampleRate, 0.02);
    _mix.setTargetValue(1.0);
}

template <typename SampleType>
void LFOGenerator<SampleType>::setFrequency(SampleType newFrequency)
{
    _frequency.setTargetValue(newFrequency);
    DBG("frequency is: " << newFrequency);
}

template <typename SampleType>
void LFOGenerator<SampleType>::setMix(SampleType newMix)
{
    _mix.setTargetValue(newMix);
}

template <typename SampleType>
void LFOGenerator<SampleType>::setLFOType(LFOType newType)
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

template class LFOGenerator<float>;
template class LFOGenerator<double>;