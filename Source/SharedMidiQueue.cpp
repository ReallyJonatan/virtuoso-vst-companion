/*
  ==============================================================================

    SharedMidiQueue.cpp
    Created: 30 Apr 2024 3:56:38pm
    Author:  Jonatan

  ==============================================================================
*/

#include "SharedMidiQueue.h"
#include "DebugUtils.h"

void SharedMidiQueue::push(const juce::MidiMessage& message, int instanceCount) {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.size() >= MAX_QUEUE_SIZE) {
		debugLog("MIDI Queue is full, dropping message");
		queue.pop();
	}
    queue.emplace(message, instanceCount);
}

void SharedMidiQueue::processAllMessages(std::function<void(juce::MidiMessage&)> processFunc) {
    std::lock_guard<std::mutex> lock(mutex);
    std::queue<MidiMessage> tempQueue;

    while (!queue.empty()) {
        auto& midiMessage = queue.front();
        processFunc(midiMessage.message);  // Process the message

        if (--midiMessage.refCount > 0) {
            tempQueue.push(std::move(midiMessage));  // Keep message if still referenced
        }
        queue.pop();
    }

    std::swap(queue, tempQueue);  // Replace old queue with the temp one that excludes processed messages
}

size_t SharedMidiQueue::size() const {
	std::lock_guard<std::mutex> lock(mutex);
	return queue.size();
}

void SharedMidiQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    debugLog("Clearing MIDI Queue, current size: " + juce::String(queue.size()));
    while (!queue.empty()) {
        queue.pop();
    }
    debugLog("Cleared MIDI Queue, new size: " + juce::String(queue.size()));
}