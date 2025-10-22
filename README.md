# RNBO_CombGrain

This is a Granular synth and feedback delay combination by RNBO (Max Msp).

By passing the grain through a feedback delay when outputting it, you can create a unique sound.

I have also arranged the code to use the [AudioProcessorValueTreeState](https://docs.juce.com/master/classAudioProcessorValueTreeState.html).
Built from the [rnbo.example.juce](https://github.com/Cycling74/rnbo.example.juce).  

For *Getting Started*, please refer to the original repository. Also included in this repository is the C++ program output from RNBO in `export/`. And Max patch is included in `patches/`.

## Demonstration
[Youtube<img width="1176" alt="grain" src="https://github.com/user-attachments/assets/02da2137-e503-4ff8-877f-4a52f74582e3" />](https://youtu.be/msMJf8HhrcQ)

## Parameter explanation
- **Mix** : You can control the ratio of dry sound to wet sound.

- **grainSpeed** : You can control the playback speed of the grain.

- **movDur** :  You can control how long it takes for a grain to move through the ring buffer.

- **reverse** : Check this box to play the grain in reverse.

- **delTime** : You can control the delay time of the feedback delay.

- **feefCoe** : Allows you to control the feedback coefficient of the feedback delay.


## Reference
- [jr-granular](https://github.com/szkkng/jr-granular)  
- [Tutorial:Saving and loading your plug-in state](https://juce.com/tutorials/tutorial_audio_processor_value_tree_state/)
