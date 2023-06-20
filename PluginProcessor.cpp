/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
// Add disto

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StutterPluginAudioProcessor::StutterPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
    , state(*this, nullptr, "STATE", {
        std::make_unique<juce::AudioParameterFloat>("gain",      "Gain",              0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("feedback",  "Delay Feedback",    0.0f, 1.0f, 0.35f),
        std::make_unique<juce::AudioParameterFloat>("mix",       "Dry/Wet",           0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("drive",     "Drive",             1.0f, 10.0f, 1.0f)
        })
{
}

StutterPluginAudioProcessor::~StutterPluginAudioProcessor()
{
}

//==============================================================================
const juce::String StutterPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StutterPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool StutterPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool StutterPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double StutterPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StutterPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int StutterPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StutterPluginAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String StutterPluginAudioProcessor::getProgramName(int index)
{
    return {};
}

void StutterPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void StutterPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    /*
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    distortion.prepare(spec);
    */

    int delayMilliseconds = 200;
    auto delaySamples = (int)std::round(sampleRate * delayMilliseconds / 1000.0);
    delayBuffer.setSize(2, delaySamples);
    delayBuffer.clear();
    delayBufferPos = 0;

}

void StutterPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StutterPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void StutterPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto& parameters = getParameters();

    
    float gain      = parameters[0]->getValue();
    float feedback  = parameters[1]->getValue();
    float mix       = parameters[2]->getValue();

    float drive     = parameters[3]->getValue();

    int delayBufferSize = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        int delayPos = delayBufferPos;

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float drySample = channelData[i];

            drySample = std::atan(drySample * 10.0f * drive);

            float delaySample = delayBuffer.getSample(channel, delayPos) * feedback;
            delayBuffer.setSample(channel, delayPos, drySample + delaySample);

            delayPos++;

            if (delayPos == delayBufferSize)
            {
                delayPos = 0;
            }

            channelData[i] = (drySample * (1.0f - mix)) + (delaySample * mix);
            channelData[i] *= gain;
        }
    }

    delayBufferPos += buffer.getNumSamples();
    if (delayBufferPos >= delayBufferSize)
    {
        delayBufferPos -= delayBufferSize;
    }

}

//==============================================================================
bool StutterPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StutterPluginAudioProcessor::createEditor()
{
    //return new StutterPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void StutterPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    if (auto xmlState = state.copyState().createXml())
        copyXmlToBinary(*xmlState, destData);
}

void StutterPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StutterPluginAudioProcessor();
}
