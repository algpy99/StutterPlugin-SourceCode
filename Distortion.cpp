/*
  ==============================================================================

    Distortion.cpp
    Created: 26 Jun 2023 9:15:25am
    Author:  goupy

  ==============================================================================
*/

#include "Distortion.h"

template <typename SampleType>
Distortion<SampleType>::Distortion()
{

}

template <typename SampleType>
void Distortion<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    _sampleRate = spec.sampleRate;
    reset();
}

template <typename SampleType>
void Distortion<SampleType>::reset()
{
    if (_sampleRate <= 0) return;

    _input.reset(_sampleRate, 0.02);
    _input.setTargetValue(0.0);

    _mix.reset(_sampleRate, 0.02);
    _mix.setTargetValue(1.0);

    _output.reset(_sampleRate, 0.02);
    _output.setTargetValue(0.0);
}

template <typename SampleType>
void Distortion<SampleType>::setDrive(SampleType newDrive) 
{
    _input.setTargetValue(newDrive);
    DBG("drive is: " << newDrive);
} 

template <typename SampleType>
void Distortion<SampleType>::setMix(SampleType newMix) 
{
    _mix.setTargetValue(newMix);
}

template <typename SampleType>
void Distortion<SampleType>::setOutput(SampleType newOutput) 
{
    _output.setTargetValue(newOutput);
}

template <typename SampleType>
void Distortion<SampleType>::setDistortionModel(DistortionModel newModel)
{
    switch (newModel)
    {
        case DistortionModel::kHard: 
        {
            _model = newModel;
            break;
        }
        case DistortionModel::kSoft: 
        {
            _model = newModel;
            break;
        }
        case DistortionModel::kSaturation: 
        {
            _model = newModel;
            break;
        }
    }
}

template class Distortion<float>;
template class Distortion<double>;