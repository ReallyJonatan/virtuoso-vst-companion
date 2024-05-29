/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DebugUtils.h"
#include "SharedResources.h"

VirtuosoCompanionAudioProcessorEditor::VirtuosoCompanionAudioProcessorEditor (VirtuosoCompanionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    debugLog("Creating Editor instance");
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (250, 250);

    int labelHeight = 25;
    int inputHeight = 50;
    int inputWidth = 100;
    int margin = 10;
    int widthMinusMargin = getWidth() - 2 * margin;

    // Listening channel label
    int listeningChannelLabelY = 50;
    listeningChannelLabel.setBounds(margin, listeningChannelLabelY, widthMinusMargin, labelHeight);
    listeningChannelLabel.setFont(juce::Font(16.0f, juce::Font::plain));
    listeningChannelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    listeningChannelLabel.setJustificationType(juce::Justification::centredLeft);
    listeningChannelLabel.setText("Listening channel", juce::dontSendNotification);
    addAndMakeVisible(listeningChannelLabel);
    
    // Listening channel box
    int channelBoxY = listeningChannelLabelY + labelHeight + margin;
    channelBox.setBounds(margin, channelBoxY, inputWidth, inputHeight);
    channelBox.setJustificationType(juce::Justification::centredLeft);
    channelBox.addItem("All", 1);
    for (int i = 1; i <= 16; i++) {
        channelBox.addItem(juce::String(i), i + 1);
    }
    channelBox.setSelectedId(audioProcessor.getSelectedChannel() + 1);
    channelBox.onChange = [this] {
		audioProcessor.setSelectedChannel(channelBox.getSelectedId() - 1);
	};
    addAndMakeVisible(channelBox);

    // Pitch wheel number label
    int pitchWheelNumberLabelY = channelBoxY + inputHeight + margin + margin;
    pitchWheelNumberLabel.setBounds(margin, pitchWheelNumberLabelY, widthMinusMargin, labelHeight);
    pitchWheelNumberLabel.setFont(juce::Font(16.0f, juce::Font::plain));
    pitchWheelNumberLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchWheelNumberLabel.setJustificationType(juce::Justification::centredLeft);
    pitchWheelNumberLabel.setText("Remap parameter to pitch wheel", juce::dontSendNotification);
    addAndMakeVisible(pitchWheelNumberLabel);

    // Pitch wheel number editor
    int pitchWheelNumberEditorY = pitchWheelNumberLabelY + labelHeight + margin;
    pitchWheelNumberEditor.setBounds(margin, pitchWheelNumberEditorY, inputWidth, labelHeight);
    pitchWheelNumberEditor.setText(juce::String(OSCtoMIDIHandler::getInstance().pitchWheelNumber));
    pitchWheelNumberEditor.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    pitchWheelNumberEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    pitchWheelNumberEditor.setJustification(juce::Justification::centredLeft);
    pitchWheelNumberEditor.onTextChange = [this] {
        OSCtoMIDIHandler::getInstance().pitchWheelNumber = pitchWheelNumberEditor.getText().getIntValue();
        };
    addAndMakeVisible(pitchWheelNumberEditor);
}

VirtuosoCompanionAudioProcessorEditor::~VirtuosoCompanionAudioProcessorEditor() {
    debugLog("Destroyed Editor instance");
}

void VirtuosoCompanionAudioProcessorEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawFittedText ("Virtuoso Companion", 0, 10, 250, 100, juce::Justification::centredTop, 1);
}

void VirtuosoCompanionAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
