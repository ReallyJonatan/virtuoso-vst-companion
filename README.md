# virtuoso-vst-companion
An open-source VST3 plugin that listens to Remote Control messages from [Virtuoso VR](https://www.virtuoso-vr.com/remote-control) and converts them to MIDI.

Built using [JUCE](https://juce.com/download/).

## Known limitations (please let me know if you find a solution or workaround for any of these):
### Ableton Live
Ableton Live has some big limitations in terms of effectively using the companion plugin, and ideally a Max for Live Plugin should be written instead to use the Virtuoso Remote Control messages.
* Ableton Live cannot learn MIDI CC from the messages produced by the companion plugin, and there is no way to manually map MIDI CC. It can only learn them from hardware controllers, Max for Live Plugins and Remote Scripts as far as I know. 
* Ableton Live cannot place the plugin before an instrument on the same channel, instead MIDI needs to be routed from the plugin channel to an instrument channel.
### FL Studio
FL Studio should be able to use most aspects of the companion plugin, but setup is quite complex.
* FL Studio cannot automatically route from the companion plugin to an instrument on the same channel either, instead MIDI needs to be routed through the plugin settings menus.
* FL Studio bundled synthesizers (such as Sytrus) do not have a plugin settings menu for MIDI routing, instead both the companion plugin and the bundled synth needs to be placed and linked inside Patcher. However, if both the plugin and the synth is placed within Patcher, the bundled synths cannot learn MIDI CC. To work around this, you need to configure the MIDI CC mappings in the synth *outside* of Patcher, save it as a preset, then load the preset inside Patcher. I have not tested this as you cannot save presets in the trial version of FL Studio, but in theory it should work.
* FL Studio does not accept pitch bend messages from the plugin, so you'll need to work around this if using MIDI CC if you want to bend pitch.
