#include <JuceHeader.h>

class WaveFormDisplay : public juce::AnimatedAppComponent
{
public:
    WaveFormDisplay(const float* dataPtr, const float* indexPtr, const int* infoPtr)
        : ringBuffer(dataPtr), ringBufferIndex(indexPtr), buffInfo{ infoPtr[0], infoPtr[1]}
    {
        buffsize = buffInfo[0];
        numChannels = buffInfo[1];
        numSamples = buffsize * numChannels;

        std::cout << "WaveFormDisplay initialized with buffSize: " << buffInfo[0]
                  << ", numChannels: " << buffInfo[1] << std::endl;
        
        setFramesPerSecond(30);
    }

    void update() override
    {
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        //draw waveform
        g.fillAll(juce::Colours::darkorange);

        if (ringBuffer == nullptr || numSamples <= 0)
            return;

        auto bounds = getLocalBounds().toFloat();
        auto midY = bounds.getCentreY();

        const float visualScaling = 0.8f; // A margin to prevent vertical penetration
        const int numLines = 100;         // Number of vertical lines to display (adjust as desired)
        const float lineSpacing = 2.0f;   // The space between lines
        const float lineWidth = (bounds.getWidth() - (lineSpacing * (numLines - 1))) / numLines;

        int totalFrames = numSamples / numChannels;
        int framesPerLine = totalFrames / numLines;

        g.setColour(juce::Colours::white);

        for (int i = 0; i < numLines; ++i)
        {
            float maxVal = 0.0f;
            int startFrame = i * framesPerLine;
            
            for (int f = 0; f < framesPerLine; ++f)
            {
                // calulate index, interleaved format
                int baseIdx = (startFrame + f) * numChannels;

                if (baseIdx + (numChannels - 1) < numSamples)
                {
                    for (int ch = 0; ch < numChannels; ++ch)
                    {
                        float absVal = std::abs(ringBuffer[baseIdx + ch]);
                        if (absVal > maxVal) maxVal = absVal;
                    }
                }
            }

            float lineHeight = maxVal * bounds.getHeight() * visualScaling;
            lineHeight = jmax(lineHeight, 2.0f); 

            float x = i * (lineWidth + lineSpacing);
            float y = midY - (lineHeight / 2.0f);

            g.fillRoundedRectangle(x, y, lineWidth, lineHeight, lineWidth * 0.5f);
        }

        //draw play position
        if (ringBufferIndex == nullptr)
            return;
        
        float halfHeight = bounds.getHeight() * 0.5f;
        float barlength = halfHeight - 20.0f;

        // Define the L channel (upper half) and R channel (lower half) regions
        auto topArea = bounds.removeFromTop(halfHeight);
        auto bottomArea = bounds;
        g.setColour(juce::Colours::grey);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            int currentSamplePos = static_cast<int>(ringBufferIndex[ch]);

            if (currentSamplePos == 0)
                barlength = 0.0f;
            
            // Calculate horizontal position (x) (normalize to 0.0 to 1.0, then multiply by width)
            float progress = currentSamplePos / static_cast<float>(buffsize);
            float x = progress * bounds.getWidth();

            float yStart = (ch == 0) ? topArea.getY() : bottomArea.getY();

            g.fillRect(x, yStart + 10.0f, 6.0f, barlength);

        }
    }


private:
    const float* ringBuffer = nullptr;
    const float* ringBufferIndex = nullptr;
    int buffInfo[2]; //0: buffSize, 1: numChannels
    int buffsize;
    int numChannels;
    int numSamples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveFormDisplay)
};