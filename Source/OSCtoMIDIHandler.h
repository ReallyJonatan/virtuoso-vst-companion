/*
  ==============================================================================

    OSCtoMIDIHandler.h
    Created: 30 Apr 2024 4:17:29pm
    Author:  Jonatan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SharedResources.h"
#include <atomic>

class OSCtoMIDIHandler : private juce::OSCReceiver, 
    private juce::OSCReceiver::Listener<juce::OSCReceiver::MessageLoopCallback>,
    private juce::Timer
{

public:
    static OSCtoMIDIHandler& getInstance();

    void registerPluginInstance();
    void unregisterPluginInstance();

    void timerCallback() override;

    int pitchWheelNumber = 0;


private:
    OSCtoMIDIHandler();
    ~OSCtoMIDIHandler();
    
    static void destroyInstance();

    static OSCtoMIDIHandler* instance;
    static std::mutex createMutex;
    
    std::atomic<int> pluginInstances{0};

    int broadcastPort = 9002;
    int connectionPort = 9003;
    juce::String localIP;
    juce::String broadcastMessage;
    juce::DatagramSocket socket;
    bool hasLoggedBroadcast = false;
    
    juce::String address;
    static const int ADDRESS_PARTS = 3;
    juce::String splitAddress[ADDRESS_PARTS];

	static const int MESSAGE_PARTS = 3;
    
	void prepareIPBroadcast();
    void broadcastLocalIP();

    void oscMessageReceived(const juce::OSCMessage& message) override;
    juce::MidiMessage convertToMidi(const juce::OSCMessage& message);
    int createSplitAddress();
	int getChannelFromMessage(const juce::OSCMessage& message);
};
