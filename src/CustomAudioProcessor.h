#include "RNBO_Utils.h"
#include "RNBO.h"
#include "RNBO_TimeConverter.h"
#include "RNBO_BinaryData.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class CustomAudioProcessor :public juce::AudioProcessor ,public juce::AudioProcessorValueTreeState::Listener{
public:
    //static CustomAudioProcessor* CreateDefault();
    CustomAudioProcessor();
    ~CustomAudioProcessor() override = default;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
   
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
  
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void setBufferData();
    void updateBuffer(float* rnboBuffer, float* rnboBufferIndex);
    //std::vector cannot handle 2D array directly
    std::vector<float> ringBuffer;
    std::vector<float> ringBufferIndex;
    const float* getRingBufferPointer() const noexcept { return &ringBuffer[0]; }
    const float* getRingBufferIndex() const noexcept { return &ringBufferIndex[0]; }

    const int* getInformationForDisplay() const noexcept { return informationForDisplay; }   

    RNBO::CoreObject& getRnboObject() { return rnboObject; }
private:
    juce::AudioProcessorValueTreeState parameters;  
    std::unordered_map<juce::String, RNBO::ParameterIndex> apvtsParamIdToRnboParamIndex;

    RNBO::CoreObject rnboObject;
    std::unique_ptr<float[]> RNBORingbuffer;
    std::unique_ptr<float[]> RNBORingbufferIndex;
    int buffSize;
    int numChannels  = 2;
    int informationForDisplay[2]; //0: buffSize, 1: numChannels
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioProcessor)
};