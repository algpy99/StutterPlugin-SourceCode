/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
// Add rev

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Distortion.h"

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
    treeState.addParameterListener("wetLevel", this);

    treeState.addParameterListener("drive", this);
    treeState.addParameterListener("mix", this);
    treeState.addParameterListener("output", this);

    treeState.addParameterListener("lfoType", this);

    treeState.addParameterListener("frequency", this);
}

StutterPluginAudioProcessor::~StutterPluginAudioProcessor()
{
    treeState.removeParameterListener("wetLevel", this);

    treeState.removeParameterListener("drive", this);
    treeState.removeParameterListener("mix", this);
    treeState.removeParameterListener("output", this);

    treeState.addParameterListener("lfoType", this);

    treeState.addParameterListener("frequency", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout StutterPluginAudioProcessor::createParameterLayout() {
    
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray lfoTypes = { "Sine", "Saw", "Square" };

    auto pWetLevel = std::make_unique<juce::AudioParameterFloat>("wetLevel", "WetLevel", 0.0f, 1.0f, 0.5f);
    
    auto pDrive = std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 24.0f, 0.0f);
    auto pMix = std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>("output", "Output", -24.0f, 24.0f, 0.0f);

    auto pLFOType = std::make_unique<juce::AudioParameterChoice>("lfoType", "LFO Type", lfoTypes, 0);

    auto pFrequency = std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", 0.0f, 20.0f, 2.0f);
    
    params.push_back(std::move(pWetLevel));

    params.push_back(std::move(pDrive));
    params.push_back(std::move(pMix));
    params.push_back(std::move(pOutput));

    params.push_back(std::move(pLFOType));

    params.push_back(std::move(pFrequency));
    
    return { params.begin(), params.end() };
}

void StutterPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateParameters();

    
    if (parameterID == "wetLevel")
    {
        wetLevel = newValue;
        DBG("wetLevel is: " << newValue);
    }

    /*
    if (parameterID == "drive")
    {
        drive = newValue;
        DBG("drive is: " << newValue);
    }
    */

    treeState.addParameterListener("wetLevel", this);

    treeState.addParameterListener("drive", this);
    treeState.addParameterListener("mix", this);
    treeState.addParameterListener("output", this);

    treeState.addParameterListener("frequency", this);
}

void StutterPluginAudioProcessor::updateParameters()
{
    
    auto type = static_cast<int>(treeState.getRawParameterValue("lfoType")->load());
    switch (type)
    {
    case 0:
        lfo.setLFOType(alex_dsp::LFOGenerator::LFOType::kSine);
        break;
    case 1:
        lfo.setLFOType(alex_dsp::LFOGenerator::LFOType::kSaw);
        break;
    case 2:
        lfo.setLFOType(alex_dsp::LFOGenerator::LFOType::kSquare);
        break;
    }

    parameters.wetLevel = wetLevel;

    reverb.setParameters(parameters);
    
    distortion.setDrive(treeState.getRawParameterValue("drive")->load());
    distortion.setMix(treeState.getRawParameterValue("mix")->load());
    distortion.setOutput(treeState.getRawParameterValue("output")->load());

    lfo.setFrequency(treeState.getRawParameterValue("frequency")->load());
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

    distortion.reset();
    distortion.prepare(spec);

    reverb.reset();
    reverb.prepare(spec);

    lfo.prepare(spec);

    updateParameters();
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);

    reverb.process(juce::dsp::ProcessContextReplacing<float>(block));

    distortion.process(juce::dsp::ProcessContextReplacing<float>(block));

    for (int ch = 0; ch < block.getNumChannels(); ++ch)
    {
        float* data = block.getChannelPointer(ch);
        for (int sample = 0; sample < block.getNumSamples(); ++sample)
        {
            lfo.process();
            data[sample] = buffer.getSample(ch, sample) * lfo.getCurrentLFOValue();

        }
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
