/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Distortion.h"
#include "LFOGenerator.h"

//==============================================================================
/**
*/
class StutterPluginAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    StutterPluginAudioProcessor();
    ~StutterPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState treeState;

private:
    //==============================================================================

    juce::Reverb::Parameters parameters;
    float wetLevel = false;

    Distortion<float> distortion;
    //float drive = false;
    //float mix = false;
    //float output = false;

    LFOGenerator<float> lfo;

    juce::dsp::Reverb reverb;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void updateParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StutterPluginAudioProcessor)
};
