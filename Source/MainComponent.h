#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ReverbProcessor.h"

//==============================================================================

class MainComponent   : public AudioAppComponent,
	                    public ChangeListener,
	                    private Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================

	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;
	TextButton processReverbButton;
	TextButton saveToFileButton;

	Slider balanceSlider;
	Label balanceLabel;
	Slider timeSlider;
	Label timeLabel;
	Slider dampeningSlider;
	Label dampeningLabel;
	Slider initSlider;
	Label initLabel;

	ToggleButton inGainOffButton;
	ToggleButton inGain5Button;
	ToggleButton inGain10Button;
	ToggleButton inGain20Button;
	ToggleButton inGain40Button;

	ToggleButton outGainOffButton;
	ToggleButton outGain5Button;
	ToggleButton outGain10Button;
	ToggleButton outGain20Button;
	ToggleButton outGain40Button;

	ToggleButton bitDepth16Button;
	ToggleButton bitDepth24Button;
	ToggleButton bitDepth32Button;

	Label inputGainLabel;
	Label outputGainLabel;
	Label clippingLabel;
	Label bitDepthLabel;
	Label wetLabel;
	Label dryLabel;

	enum RadioButtonIDs
	{
		inputGainButtons = 1,
		outputGainButtons = 2,
		bitDepthButtons = 3
	};

	enum TransportState
	{
		Playing,
		Stopped
	};

	AudioFormatManager formatManager;
	AudioBuffer <float> fileBuffer;
	int bitDepth;
	int position;
	TransportState state;
	AudioDeviceManager deviceManager;
	AudioDeviceManager::AudioDeviceSetup adsetup;
	Atomic <bool> stopFlag;
	AudioThumbnailCache thumbnailCache;
	AudioThumbnail thumbnail;

	ReverbProcessor reverb;

	void openButtonClicked(void);
	void playButtonClicked(void);
	void stopButtonClicked(void);

	void processReverbButtonClicked(void);
	void saveToFileButtonClicked(void);

	void changeListenerCallback(ChangeBroadcaster* source) override;
	void timerCallback(void) override;

	void changeState(TransportState newState);

	void thumbnailChanged(void);
	void paintIfNoFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds);
	void paintIfFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
