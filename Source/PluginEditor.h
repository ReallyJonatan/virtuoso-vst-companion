/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SharedResources.h"

class VirtuosoCompanionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VirtuosoCompanionAudioProcessorEditor (VirtuosoCompanionAudioProcessor&);
    ~VirtuosoCompanionAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VirtuosoCompanionAudioProcessor& audioProcessor;
    
    juce::Label listeningChannelLabel;
    juce::ComboBox channelBox;
    juce::Label pitchWheelNumberLabel;
    juce::TextEditor pitchWheelNumberEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtuosoCompanionAudioProcessorEditor)
};
