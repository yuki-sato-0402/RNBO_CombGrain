#include "JuceHeader.h"
#include "RNBO.h"
#include "WaveFormDisplay.h"

class CustomAudioEditor : public juce::AudioProcessorEditor
{
public:
    CustomAudioEditor(CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~CustomAudioEditor() override = default;
    void paint (Graphics& g) override;
    void resized() override; 
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

private:
    juce::AudioProcessorValueTreeState& valueTreeState; // ✅ Hold by reference
    CustomAudioProcessor& audioProcessor;
    
    juce::Slider MixDial;
    juce::Slider grainSizeDial;
    juce::Slider densityDial;
    juce::Slider movDurDial;
    juce::Slider delTimeDial;
    juce::Slider feedCoeDial;

    juce::ToggleButton reverseButton;

    juce::Label  MixLabel;
    juce::Label  grainSizeLabel;
    juce::Label  densityLabel;
    juce::Label  movDurLabel;
    juce::Label  delTimeLabel;
    juce::Label  feedCoeLabel;

    std::unique_ptr<SliderAttachment> MixDialAttachment;
    std::unique_ptr<SliderAttachment> grainSizeDialAttachment;
    std::unique_ptr<SliderAttachment> densityDialAttachment;
    std::unique_ptr<SliderAttachment> movDurDialAttachment;
    std::unique_ptr<SliderAttachment> delTimeDialAttachment;
    std::unique_ptr<SliderAttachment> feedCoeDialAttachment;
    std::unique_ptr<ButtonAttachment> reverseButtonAattachment;

    WaveFormDisplay waveFormDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
