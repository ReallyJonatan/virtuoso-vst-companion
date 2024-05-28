/*
  ==============================================================================

    SharedMidiQueue.h
    Created: 30 Apr 2024 3:56:38pm
    Author:  Jonatan

  ==============================================================================
*/

#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>

struct MidiMessage {
    juce::MidiMessage message;
    std::atomic<int> refCount;

    // Constructor
    MidiMessage(const juce::MidiMessage& msg, int count)
        : message(msg), refCount(count) {}

    // Delete copy constructor and copy assignment to prevent copying
    MidiMessage(const MidiMessage&) = delete;
    MidiMessage& operator=(const MidiMessage&) = delete;

    // Move constructor
    MidiMessage(MidiMessage&& other) noexcept
        : message(std::move(other.message)), refCount(other.refCount.load(std::memory_order_relaxed)) {}

    // Move assignment operator
    MidiMessage& operator=(MidiMessage&& other) noexcept {
        if (this != &other) {
            message = std::move(other.message);
            refCount.store(other.refCount.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }
};

class SharedMidiQueue {
public:
    void push(const juce::MidiMessage& message, int instanceCount);
    void processAllMessages(std::function<void(juce::MidiMessage&)> processFunc);
    size_t size() const;
    void clear();

private:
    static const size_t MAX_QUEUE_SIZE = 128;
    std::queue<MidiMessage> queue;
    mutable std::mutex mutex;
};
