#include "JuceHeader.h"
#include "RNBO.h"

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
    
    juce::Slider dial1Slider;
    juce::Slider dial2Slider;
    juce::Slider dial3Slider;
    juce::Slider dial4Slider;
    juce::Slider dial5Slider;

    juce::ToggleButton reverseButton;

    juce::Label  label1;
    juce::Label  label2;
    juce::Label  label3;
    juce::Label  label4;
    juce::Label  label5;

    std::unique_ptr<SliderAttachment> dial1Attachment;
    std::unique_ptr<SliderAttachment> dial2Attachment;
    std::unique_ptr<SliderAttachment> dial3Attachment;
    std::unique_ptr<SliderAttachment> dial4Attachment;
    std::unique_ptr<SliderAttachment> dial5Attachment;
    std::unique_ptr<ButtonAttachment> reverseButtonAattachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
