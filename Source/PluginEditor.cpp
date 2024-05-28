/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DebugUtils.h"

VirtuosoCompanionAudioProcessorEditor::VirtuosoCompanionAudioProcessorEditor (VirtuosoCompanionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    debugLog("Creating Editor instance");
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (250, 200);

    lastMessageLabel.setBounds(10, 40, 230, 50);
    lastMessageLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lastMessageLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    lastMessageLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lastMessageLabel);

    channelBox.setBounds(10, 80, 230, 50);
    channelBox.addItem("All", 1);
    for (int i = 1; i <= 16; i++) {
        channelBox.addItem(juce::String(i), i + 1);
    }
    channelBox.setSelectedId(audioProcessor.getSelectedChannel() + 1);
    channelBox.onChange = [this] {
		audioProcessor.setSelectedChannel(channelBox.getSelectedId() - 1);
	};
    addAndMakeVisible(channelBox);
}

VirtuosoCompanionAudioProcessorEditor::~VirtuosoCompanionAudioProcessorEditor() {
    debugLog("Destroyed Editor instance");
}

void VirtuosoCompanionAudioProcessorEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Virtuoso Companion", 0, 10, 250, 100, juce::Justification::centredTop, 1);
}

void VirtuosoCompanionAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
