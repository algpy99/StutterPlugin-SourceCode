/*
  ==============================================================================

    LFOGenerator.h
    Created: 27 Jun 2023 3:40:37pm
    Author:  goupy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class LFOGenerator
{
public:
    LFOGenerator();

    void prepare(juce::dsp::ProcessSpec& spec);
    void reset();

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample(inputSamples[i]);
        }

    };

    SampleType processSample(SampleType inputSample) noexcept
    {
        switch (_type)
        {
            case LFOType::kSine:
            {
                return processSine(inputSample);
                break;
            }
            case LFOType::kSaw:
            {
                return processSaw(inputSample);
                break;
            }
            case LFOType::kSquare:
            {
                return processSquare(inputSample);
                break;
            }
        }
    };

    SampleType processSine(SampleType inputSample)
    {
        float _depth = 1;
        float _frequency = 20;
        float _samplingFrequency = 44100;
        float _time = 0;
        float _delta = 1/ _samplingFrequency;
        float modulator = 1 + _depth * sin(2 * juce::MathConstants<float>::pi * _frequency * _time); // frequency between 0 and 20Hz
        _time += _delta;
        inputSample *= modulator;
        DBG("modulator is: " << modulator);

        return inputSample;
    }

    SampleType processSaw(SampleType inputSample)
    {
        return inputSample;
    }

    SampleType processSquare(SampleType inputSample)
    {
        return inputSample;
    }

    enum class LFOType
    {
        kSine,
        kSaw,
        kSquare
    };

    void setFrequency(SampleType newFrequency);
    void setMix(SampleType newMix);

    void setLFOType(LFOType newType);

    void getCurrentLFOValue(SampleType newvalue);

private:
    //juce::SmoothedValue<float> _frequency = 50;
    //juce::SmoothedValue<float> _time = 10;
    //juce::SmoothedValue<float> _mix;

    float _sampleRate = 44100.0f;

    LFOType _type = LFOType::kSine;
};