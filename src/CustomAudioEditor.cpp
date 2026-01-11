#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor (&p), valueTreeState(vts), audioProcessor(p), waveFormDisplay (p.getRingBufferPointer(), p.getRingBufferIndex(), p.getInformationForDisplay())
{
    addAndMakeVisible(waveFormDisplay);

    addAndMakeVisible(reverseButton);
    reverseButton.setButtonText("reverse");
    reverseButtonAattachment.reset (new ButtonAttachment (valueTreeState, "reverse", reverseButton));
    
    addAndMakeVisible(MixDial);
    MixDialAttachment.reset (new SliderAttachment (valueTreeState, "Mix", MixDial));
    MixDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    MixDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, MixDial.getTextBoxWidth(), MixDial.getTextBoxHeight());
    MixDial.setTextValueSuffix (" %");
    MixDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    MixDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    MixDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    MixDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    MixDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    
    addAndMakeVisible(MixLabel);
    addAndMakeVisible(MixLabel);
    MixLabel.setText ("Mix", juce::dontSendNotification);
    MixLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(grainSizeDial);
    grainSizeDialAttachment.reset (new SliderAttachment (valueTreeState, "grainSize", grainSizeDial));
    grainSizeDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    grainSizeDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, grainSizeDial.getTextBoxWidth(), grainSizeDial.getTextBoxHeight());
    grainSizeDial.setTextValueSuffix (" ms");
    grainSizeDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    grainSizeDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    grainSizeDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    grainSizeDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    grainSizeDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
   
    addAndMakeVisible(grainSizeLabel);
    grainSizeLabel.setText ("grainSize", juce::dontSendNotification);
    grainSizeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(densityDial);
    densityDialAttachment.reset (new SliderAttachment (valueTreeState, "density", densityDial));
    densityDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    densityDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, densityDial.getTextBoxWidth(), densityDial.getTextBoxHeight());
    densityDial.setTextValueSuffix (" %");
    densityDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    densityDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    densityDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    densityDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    densityDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white); 

    addAndMakeVisible(densityLabel);
    densityLabel.setText ("density", juce::dontSendNotification);
    densityLabel.setJustificationType(juce::Justification::centred);
    

    addAndMakeVisible(movDurDial);
    movDurDialAttachment.reset (new SliderAttachment (valueTreeState, "movDur", movDurDial));
    movDurDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    movDurDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, movDurDial.getTextBoxWidth(), movDurDial.getTextBoxHeight());
    movDurDial.setTextValueSuffix (" ms");
    movDurDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    movDurDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    movDurDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    movDurDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    movDurDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    

    addAndMakeVisible(movDurLabel);
    movDurLabel.setText ("movDur", juce::dontSendNotification);
    movDurLabel.setJustificationType(juce::Justification::centred);


    addAndMakeVisible(delTimeDial);
    delTimeDialAttachment.reset (new SliderAttachment (valueTreeState, "delTime", delTimeDial));
    delTimeDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    delTimeDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, delTimeDial.getTextBoxWidth(), delTimeDial.getTextBoxHeight());
    delTimeDial.setTextValueSuffix (" ms");
    delTimeDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    delTimeDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    delTimeDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    delTimeDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    delTimeDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(delTimeLabel);
    delTimeLabel.setText ("delTime", juce::dontSendNotification);
    delTimeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(feedCoeDial);
    feedCoeDialAttachment.reset (new SliderAttachment (valueTreeState, "feedCoe", feedCoeDial));
    feedCoeDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);     
    feedCoeDial.setTextBoxStyle (juce::Slider::TextBoxBelow, false, feedCoeDial.getTextBoxWidth(), feedCoeDial.getTextBoxHeight());
    feedCoeDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    feedCoeDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    feedCoeDial.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange.withAlpha(0.75f));
    feedCoeDial.setColour(juce::Slider::thumbColourId , juce::Colours::darkorange.brighter(1.5));
    feedCoeDial.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    feedCoeDial.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(feedCoeLabel);
    feedCoeLabel.setText ("feedCoe", juce::dontSendNotification);
    feedCoeLabel.setJustificationType(juce::Justification::centred);
    setSize(600, 500);
}


void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::darkorange); 
}

void CustomAudioEditor::resized()
{
    auto area = getLocalBounds();

    const int componentWidth1 = (area.getWidth() - 100)/4;
    const int componentHeight = (area.getHeight() - 60) / 4;
    const int padding = 20;

    waveFormDisplay.setBounds(padding, padding, area.getWidth() - (padding * 2), area.getHeight() / 2 - (padding * 2));

    MixDial.setBounds(padding, waveFormDisplay.getBottom() + padding, componentWidth1 ,  componentHeight);
    grainSizeDial.setBounds(MixDial.getRight() + padding, waveFormDisplay.getBottom() + padding, componentWidth1 ,  componentHeight);
    densityDial.setBounds(grainSizeDial.getRight() + padding,  waveFormDisplay.getBottom() + padding, componentWidth1 ,  componentHeight);
    movDurDial.setBounds(densityDial.getRight() + padding,  waveFormDisplay.getBottom() + padding, componentWidth1 ,  componentHeight);
    reverseButton.setBounds(padding,  MixDial.getBottom() + padding, componentWidth1 ,  componentHeight);
    delTimeDial.setBounds(reverseButton.getRight()  + padding, MixDial.getBottom() + padding, componentWidth1 ,  componentHeight);
    feedCoeDial.setBounds(delTimeDial.getRight() + padding,  MixDial.getBottom() + padding, componentWidth1 ,  componentHeight);

    MixLabel.setBounds(MixDial.getX(), MixDial.getY()-10, MixDial.getWidth(),MixDial.getTextBoxHeight() );
    grainSizeLabel.setBounds(grainSizeDial.getX(), grainSizeDial.getY()-10, grainSizeDial.getWidth(),grainSizeDial.getTextBoxHeight() );
    densityLabel.setBounds(densityDial.getX(), densityDial.getY()-10, densityDial.getWidth(),densityDial.getTextBoxHeight() );
    movDurLabel.setBounds(movDurDial.getX(), movDurDial.getY()-10, movDurDial.getWidth(),movDurDial.getTextBoxHeight() );
    delTimeLabel.setBounds(delTimeDial.getX(), delTimeDial.getY()-10, delTimeDial.getWidth(),delTimeDial.getTextBoxHeight() );
    feedCoeLabel.setBounds(feedCoeDial.getX(), feedCoeDial.getY()-10, feedCoeDial.getWidth(),feedCoeDial.getTextBoxHeight() );
  
}
