/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
// Add rev

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
    ), treeState (*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("roomSize", this);
    treeState.addParameterListener("wetLevel", this);
    treeState.addParameterListener("width", this);    

    /*
    float roomSize   = 0.5f;     /**< Room size, 0 to 1.0, where 1.0 is big, 0 is small. 
    float damping = 0.5f;     /**< Damping, 0 to 1.0, where 0 is not damped, 1.0 is fully damped. 
    float wetLevel = 0.33f;    /**< Wet level, 0 to 1.0 
    float dryLevel = 0.4f;     /**< Dry level, 0 to 1.0 
    float width = 1.0f;     /**< Reverb width, 0 to 1.0, where 1.0 is very wide. 
    float freezeMode = 0.0f;     /**< Freeze mode - values < 0.5 are "normal" mode, values > 0.5
                                      put the reverb into a continuous feedback loop.
    */
}

StutterPluginAudioProcessor::~StutterPluginAudioProcessor()
{
    treeState.removeParameterListener("roomSize", this);
    treeState.removeParameterListener("wetLevel", this);
    treeState.removeParameterListener("width", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout StutterPluginAudioProcessor::createParameterLayout() {
    
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    
    auto pRoomSize = std::make_unique<juce::AudioParameterFloat>("roomSize", "RoomSize", 0.0f, 1.0f, 0.5f);
    auto pWetLevel = std::make_unique<juce::AudioParameterFloat>("wetLevel", "WetLevel", 0.0f, 1.0f, 0.5f);
    auto pWidth = std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.5f);
    
    params.push_back(std::move(pRoomSize));
    params.push_back(std::move(pWetLevel));
    params.push_back(std::move(pWidth));
    

    return { params.begin(), params.end() };
}

void StutterPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "roomSize")
    {
        roomSize = newValue;
        DBG("roomSize is: " << newValue);
    }

    if (parameterID == "wetLevel")
    {
        wetLevel = newValue;
        DBG("wetLevel is: " << newValue);
    }

    if (parameterID == "width")
    {
        width = newValue;
        DBG("width is: " << newValue);
    }
    treeState.addParameterListener("roomSize", this);
    treeState.addParameterListener("wetLevel", this);
    treeState.addParameterListener("width", this);
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
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    reverb.reset();
    reverb.prepare(spec);

    /*
    int delayMilliseconds = 200;
    auto delaySamples = (int)std::round(sampleRate * delayMilliseconds / 1000.0);
    delayBuffer.setSize(2, delaySamples);
    delayBuffer.clear();
    delayBufferPos = 0;
    */

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

    juce::dsp::AudioBlock<float> block {buffer};

    parameters.roomSize = roomSize;
    parameters.wetLevel = wetLevel;
    parameters.width = width;

    reverb.setParameters(parameters);

    reverb.process(juce::dsp::ProcessContextReplacing<float>(block));

    /*
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = block.getChannelPointer(channel);

        for (int i = 0; i < block.getNumSamples(); ++i)
        {
            
        }
    }
    */
    

    /*
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
    */

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
    /*
    treeState.state.appendChild(variableTree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
    */
}

void StutterPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    /*
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    variableTree = tree.getChildWithName("Variables");

    if (tree.isValid())
    {
        treeSate.state = tree;
    }
    */

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StutterPluginAudioProcessor();
}
