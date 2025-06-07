#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), valueTreeState(vts), audioProcessor(p)// 参照メンバーを初期化（必須）
{
    addAndMakeVisible(reverseButton);
    reverseButton.setButtonText("reverse");
    reverseButtonAattachment.reset (new ButtonAttachment (valueTreeState, "reverse", reverseButton));
    
    addAndMakeVisible(dial1Slider);
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "Mix", dial1Slider));
    dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial1Slider.getTextBoxHeight());
    dial1Slider.setTextValueSuffix (" %");
    dial1Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial1Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    dial1Slider.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    dial1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial1Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    
    addAndMakeVisible(label1);
    label1.setText ("Mix", juce::dontSendNotification);
    label1.setJustificationType(juce::Justification::centred);
    

    addAndMakeVisible(dial2Slider);
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "grainSpeed", dial2Slider));
    dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
    dial2Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial2Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    dial2Slider.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    dial2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial2Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
   

    addAndMakeVisible(label2);
    label2.setText ("grainSpeed", juce::dontSendNotification);
    label2.setJustificationType(juce::Justification::centred);
    

    addAndMakeVisible(dial3Slider);
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "movDur", dial3Slider));
    dial3Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    dial3Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial3Slider.getTextBoxWidth(), dial3Slider.getTextBoxHeight());
    dial1Slider.setTextValueSuffix (" ms");
    dial3Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial3Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    dial3Slider.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    dial3Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial3Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    

    addAndMakeVisible(label3);
    label3.setText ("movDur", juce::dontSendNotification);
    label3.setJustificationType(juce::Justification::centred);


  addAndMakeVisible(dial4Slider);
    dial4Attachment.reset (new SliderAttachment (valueTreeState, "delTime", dial4Slider));
    dial4Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    dial4Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial4Slider.getTextBoxWidth(), dial4Slider.getTextBoxHeight());
    dial4Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial4Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    dial4Slider.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    dial4Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial4Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
 
    addAndMakeVisible(label4);
    label4.setText ("delTime", juce::dontSendNotification);
    label4.setJustificationType(juce::Justification::centred);


    addAndMakeVisible(dial5Slider);
    dial5Attachment.reset (new SliderAttachment (valueTreeState, "FeedCoe", dial5Slider));
    dial5Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    dial5Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial5Slider.getTextBoxWidth(), dial5Slider.getTextBoxHeight());
    dial5Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial5Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial5Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    dial5Slider.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    dial5Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial5Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    
    addAndMakeVisible(label5);
    label5.setText ("FeedCoe", juce::dontSendNotification);
    label5.setJustificationType(juce::Justification::centred);

    setSize(600, 500);
}


void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::darkorange); // 背景を黒に設定
}

void CustomAudioEditor::resized()
{
    //エディター全体の領域を取得
    auto area = getLocalBounds();

    const int componentWidth1 = (area.getWidth() - 80)/3;
    const int componentHeight = (area.getHeight() - 120) / 2;
    const int paddingW = 20;
    const int paddingH = 40;

    dial1Slider.setBounds(paddingW,  paddingH , componentWidth1 ,  componentHeight);
    dial2Slider.setBounds(dial1Slider.getRight() + paddingW, paddingH, componentWidth1 ,  componentHeight);
    dial3Slider.setBounds(dial2Slider.getRight() + paddingW,  paddingH, componentWidth1 ,  componentHeight);

    reverseButton.setBounds(paddingW,  dial1Slider.getBottom() + paddingH, componentWidth1 ,  componentHeight);
    dial4Slider.setBounds(reverseButton.getRight()  + paddingW, dial1Slider.getBottom() + paddingH, componentWidth1 ,  componentHeight);
    dial5Slider.setBounds(dial4Slider.getRight() + paddingW,  dial1Slider.getBottom() + paddingH, componentWidth1 ,  componentHeight);


    label1.setBounds(dial1Slider.getX(), dial1Slider.getY()-20, dial1Slider.getWidth(),dial1Slider.getTextBoxHeight() );
    label2.setBounds(dial2Slider.getX(), dial2Slider.getY()-20, dial2Slider.getWidth(),dial2Slider.getTextBoxHeight() );
    label3.setBounds(dial3Slider.getX(), dial3Slider.getY()-20, dial3Slider.getWidth(),dial3Slider.getTextBoxHeight() );
    label4.setBounds(dial4Slider.getX(), dial4Slider.getY()-20, dial4Slider.getWidth(),dial4Slider.getTextBoxHeight() );
    label5.setBounds(dial5Slider.getX(), dial5Slider.getY()-20, dial5Slider.getWidth(),dial5Slider.getTextBoxHeight() );

}
