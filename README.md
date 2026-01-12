# RNBO_CombGrain

This is a Granular synth and feedback delay combination by RNBO (Max Msp).

By passing the grain through a feedback delay when outputting it, you can create a unique sound.

I have also arranged the code to use the [AudioProcessorValueTreeState](https://docs.juce.com/master/classAudioProcessorValueTreeState.html).
Built from the [rnbo.example.juce](https://github.com/Cycling74/rnbo.example.juce).  

For *Getting Started*, please refer to the original repository. Also included in this repository is the C++ program output from RNBO in `export/`. And Max patch is included in `patches/`.

## Demonstration
[Youtube<img width="712" height="640" alt="Screenshot 2026-01-12 at 17 28 26" src="https://github.com/user-attachments/assets/91930cce-f179-4618-b5ac-eff9e88445af" />](https://youtu.be/6N8od3bVpZA?si=NGJOEIaaf8gUuiXw)

## Parameter explanation
- **Mix** : You can control the ratio of dry sound to wet sound.

- **grainSize** : You can control the size of the grain.

- **density** : You can control the grain density (playback frequency).

- **movDur** :  You can control how long it takes for a grain to move through the ring buffer.

- **reverse** : Check this box to play the grain in reverse.

- **delTime** : You can control the delay time of the feedback delay.

- **feefCoe** : Allows you to control the feedback coefficient of the feedback delay.


## Reference
- [jr-granular](https://github.com/szkkng/jr-granular)  
- [Tutorial:Saving and loading your plug-in state](https://juce.com/tutorials/tutorial_audio_processor_value_tree_state/)
