/*
  ==============================================================================

    OSCtoMIDIHandler.cpp
    Created: 30 Apr 2024 4:17:29pm
    Author:  Jonatan

  ==============================================================================
*/

#include "OSCtoMIDIHandler.h"
#include "DebugUtils.h"

OSCtoMIDIHandler::OSCtoMIDIHandler() : socket(true) {
    debugLog("Creating OSCtoMIDIHandler instance");
    addListener(this);
    prepareIPBroadcast();
}

OSCtoMIDIHandler::~OSCtoMIDIHandler() {
    removeListener(this);
    socket.shutdown();
    debugLog("Destroyed OSCtoMIDIHandler instance");
}

OSCtoMIDIHandler* OSCtoMIDIHandler::instance = nullptr;
std::mutex OSCtoMIDIHandler::createMutex;

OSCtoMIDIHandler& OSCtoMIDIHandler::getInstance() {
    std::lock_guard<std::mutex> lock(createMutex);
    if (!instance) {
        instance = new OSCtoMIDIHandler();
    }
    return *instance;
}

void OSCtoMIDIHandler::destroyInstance() {
    std::lock_guard<std::mutex> lock(createMutex);
    delete instance;
    instance = nullptr;
}

void OSCtoMIDIHandler::registerPluginInstance() {
    pluginInstances.fetch_add(1);
    debugLog("Plugin instance added. Current number of instances : " + juce::String(pluginInstances));
    if (pluginInstances == 1) {
		debugLog("First plug-in instance using OSCtoMIDIHandler - starting up...");
        broadcastLocalIP();
        startTimer(2000);
        
        if (connect(connectionPort)) {
            debugLog("Connected OSC Receiver on port " + juce::String(connectionPort));
		}
        else {
            debugLog("Failed to connect OSC Receiver on port " + juce::String(connectionPort));
        }
    }
}

void OSCtoMIDIHandler::unregisterPluginInstance() {
    pluginInstances.fetch_sub(1);
    debugLog("Plugin instance removed. Current number of instances : " + juce::String(pluginInstances));
    if (pluginInstances == 0) {
        debugLog("No plug-in instances using OSCtoMIDIHandler - cleaning up...");
        stopTimer();
		disconnect();
		sharedMidiQueue.clear();
		destroyInstance();
	}
}

void OSCtoMIDIHandler::prepareIPBroadcast() {
    localIP = juce::IPAddress::getLocalAddress().toString();
    broadcastMessage = "VirtuosoCompanion:" + localIP;
	if (socket.bindToPort(0)) {
		debugLog("Bound to port " + juce::String(socket.getBoundPort()));
	}
	else {
		debugLog("Failed to bind to port");
	}
}

void OSCtoMIDIHandler::broadcastLocalIP() {
    bool result = socket.write("255.255.255.255", broadcastPort, broadcastMessage.toRawUTF8(), broadcastMessage.getNumBytesAsUTF8());
    if (result && !hasLoggedBroadcast) {
		debugLog("Successfully sent first of intermittent UDP broadcasts with message: " + broadcastMessage);
		hasLoggedBroadcast = true;
    }
    else if (!result) {
        debugLog("Failed to send UDP broadcast");
    }
}

void OSCtoMIDIHandler::timerCallback() {
	broadcastLocalIP();
}

void OSCtoMIDIHandler::oscMessageReceived(const juce::OSCMessage& oscMessage) {
    address = oscMessage.getAddressPattern().toString();
    int splitAddressLength = createSplitAddress();
    
	// Check for invalid and non-midi messages
    if (splitAddressLength != ADDRESS_PARTS) {
        debugLog("Invalid address length: " + address);
        return;
    }
    if (splitAddress[2] == "connect") {
        debugLog("Received connection message from Virtuoso, client name: " + splitAddress[0]);
        return;
    }
    if (splitAddress[2] == "disconnect") {
        debugLog("Received disconnection message from Virtuoso, client name: " + splitAddress[0]);
        return;
    }
    if (oscMessage.size() != MESSAGE_PARTS) {
        debugLog("Invalid message size sent to address: " + address);
        return;
    }

    auto midiMessage = convertToMidi(oscMessage);
    if (midiMessage.isSysEx()) { 
        // Empty midi messages will return true as being Sysex, so we use this as a hack to skip adding to queue
		return;
	}
    sharedMidiQueue.push(midiMessage, pluginInstances);
}

juce::MidiMessage OSCtoMIDIHandler::convertToMidi(const juce::OSCMessage& oscMessage) {
    if (splitAddress[2] == "noteon") {
        int velocity = std::clamp(static_cast<int>(oscMessage[0].getFloat32() * 128), 0, 127);
        int channel = getChannelFromMessage(oscMessage);
        int noteNumber = std::clamp(oscMessage[2].getInt32(), 0, 127);
        debugLog("Note on " + juce::String(noteNumber) + " on channel " + juce::String(channel) + " with velocity " + juce::String(velocity));
        return juce::MidiMessage::noteOn(channel, noteNumber, (juce::uint8)velocity);
    }
    else if (splitAddress[2] == "noteoff") {
        int channel = getChannelFromMessage(oscMessage);
        int noteNumber = std::clamp(oscMessage[2].getInt32(), 0, 127);
        debugLog("Note off " + juce::String(noteNumber) + " on channel " + juce::String(channel));
        return juce::MidiMessage::noteOff(channel, noteNumber, (juce::uint8)0);
    }
    else if (splitAddress[2] == "parameter") {
		int parameterValue = std::clamp(static_cast<int>(oscMessage[0].getFloat32() * 128), 0, 127);
        int channel = getChannelFromMessage(oscMessage);
		int controllerNumber = std::clamp(oscMessage[2].getInt32(), 0, 127);
		// NOTE: Here we might want to convert Virtuoso controller numbers to sensible MIDI CC numbers, 
        // as many MIDI CC numbers might be reserved for specific purposes
        if (controllerNumber == 0) { // Remap 0 to pitch wheel
            int pitchValue = std::clamp(static_cast<int>(oscMessage[0].getFloat32() * 16384), 0, 16383);
            return juce::MidiMessage::pitchWheel(channel, pitchValue);
        }
        return juce::MidiMessage::controllerEvent(channel, controllerNumber, parameterValue);
    }
	else if (splitAddress[2] == "volume" && splitAddress[1] != "global") {
		// Example, we can use controller 7 for volume as that is the standard volume controller
		// We skip the global volume controller as it is not implemented here
		int volumeValue = std::clamp(static_cast<int>(oscMessage[0].getFloat32() * 128), 0, 127);
        int channel = getChannelFromMessage(oscMessage);
		return juce::MidiMessage::controllerEvent(channel, 7, volumeValue);
	}
    else {
        // Other messages are sent from Virtuoso over OSC but are not implemented here
		// For a hopefully complete list, see the Virtuoso website and API documentation
        debugLog("Message address not implemented: " + address);
	    return juce::MidiMessage();
    }
}

int OSCtoMIDIHandler::createSplitAddress() {
    // Split address tree into parts
    int lastDividerIndex = 0;
    int nextDividerIndex = 0;
    int splitAddressLength = 0;
    for (int i = 0; i < ADDRESS_PARTS; i++) {
        splitAddressLength++;
        nextDividerIndex = address.indexOf(lastDividerIndex + 1, "/");
        if (nextDividerIndex == -1) {
            splitAddress[i] = address.substring(lastDividerIndex + 1);
            return splitAddressLength;
        }
        else {
            splitAddress[i] = address.substring(lastDividerIndex + 1, nextDividerIndex);
        }
        lastDividerIndex = nextDividerIndex;
    }
    return splitAddressLength;
}

int OSCtoMIDIHandler::getChannelFromMessage(const juce::OSCMessage& oscMessage) {
    return std::clamp(oscMessage[1].getInt32(), 1, 16);
}
