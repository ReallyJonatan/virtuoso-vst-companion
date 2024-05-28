# virtuoso-vst-companion
An open-source VST3 plugin that listens to Remote Control messages from [Virtuoso VR](https://www.virtuoso-vr.com/remote-control) and converts them to MIDI.

Built using [JUCE](https://juce.com/download/).

## How to use
1) Download the plugin, put it in your VST3 plugins folder, and run your music program.

2) Make sure the device running Virtuoso and the computer running your music program are on the same local network, or is the same computer.

3) Put the Virtuoso Companion VST on a track in your music program, and route the MIDI to a synthesizer or sampler in the program. How to route MIDI depends on what program you are using, in some, like Reaper, you can just put the Virtuoso Companion on the same track as the synthesizer or sampler, right before it in the FX chain. In others, it is more complicated - see Known limitations below.

4) Make sure you allow the plugin to access the network if you get a firewall warning.

5) Start Virtuoso, go to Track Settings and click "Enable Remote Control".

6) If you're running Virtuoso on a Windows computer, you may need to allow access to network again if you get a firewall warning.

7) Play on an instrument in Virtuoso. The synthesizer or sampler in your music program should now play sound. If not, check that you've routed MIDI correctly for your program, and that sounds from the synthesizer or sampler can be heard if you trigger it using some other method.

## Known limitations
Here's a list of the known limitations of using the companion plugin. Please let me know if you find a solution or workaround for any of these, and feel free to fork the plugin or do a pull request if you solve it in code.
### Ableton Live
Ableton Live has some big limitations in terms of effectively using the companion plugin, and ideally a Max for Live Plugin should be written instead to use the Virtuoso Remote Control messages.
* Ableton Live cannot learn MIDI CC from the messages produced by the companion plugin, and there is no way to manually map MIDI CC. It can only learn them from hardware controllers, Max for Live Plugins and Remote Scripts as far as I know. 
* Ableton Live cannot place the plugin before an instrument on the same channel, instead MIDI needs to be routed from the plugin channel to an instrument channel.
### FL Studio
FL Studio should be able to use most aspects of the companion plugin, but setup is quite complex.
* FL Studio cannot automatically route from the companion plugin to an instrument on the same channel either, instead MIDI needs to be routed through the plugin settings menus.
* FL Studio bundled synthesizers (such as Sytrus) do not have a plugin settings menu for MIDI routing, instead both the companion plugin and the bundled synth needs to be placed and linked inside Patcher. However, if both the plugin and the synth is placed within Patcher, the bundled synths cannot learn MIDI CC. To work around this, you need to configure the MIDI CC mappings in the synth *outside* of Patcher, save it as a preset, then load the preset inside Patcher. I have not tested this as you cannot save presets in the trial version of FL Studio, but in theory it should work.
* FL Studio does not accept pitch bend messages from the plugin, so you'll need to work around this if using MIDI CC if you want to bend pitch.
