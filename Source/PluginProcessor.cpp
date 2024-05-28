/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DebugUtils.h"

VirtuosoCompanionAudioProcessor::VirtuosoCompanionAudioProcessor() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    debugLog("Creating Processor instance");
}

VirtuosoCompanionAudioProcessor::~VirtuosoCompanionAudioProcessor()
{
    debugLog("Destroyed Processor instance");
}

const juce::String VirtuosoCompanionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VirtuosoCompanionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VirtuosoCompanionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VirtuosoCompanionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VirtuosoCompanionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VirtuosoCompanionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VirtuosoCompanionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VirtuosoCompanionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VirtuosoCompanionAudioProcessor::getProgramName (int index)
{
    return {};
}

void VirtuosoCompanionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void VirtuosoCompanionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	if (!isInitialized) {
        // Register this plugin instance with the OSCtoMIDIHandler
		OSCtoMIDIHandler::getInstance().registerPluginInstance();
		isInitialized = true;
	}
}

void VirtuosoCompanionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    if (isInitialized) {
		// Unregister this plugin instance with the OSCtoMIDIHandler
		OSCtoMIDIHandler::getInstance().unregisterPluginInstance();
		isInitialized = false;
	}
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VirtuosoCompanionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void VirtuosoCompanionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    
    // Process MIDI messages
    sharedMidiQueue.processAllMessages([&](juce::MidiMessage& message) {
        if (selectedChannel == 0 || message.getChannel() == selectedChannel) {
            midiMessages.addEvent(message, 0);
		}
    });
}

bool VirtuosoCompanionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VirtuosoCompanionAudioProcessor::createEditor()
{
    return new VirtuosoCompanionAudioProcessorEditor (*this);
}

void VirtuosoCompanionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, false);
    stream.writeInt(1); // Version number
    stream.writeInt(selectedChannel.load());
}

void VirtuosoCompanionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    int version = stream.readInt();  // Read the version number
    if (version == 1) {
        int savedChannel = stream.readInt();  // Read the selected channel as an integer
        setSelectedChannel(savedChannel);  // Restore the channel setting
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VirtuosoCompanionAudioProcessor();
}