#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : juce::AudioAppComponent(deviceManager),
                                 thumbnailCache(5),
	                             thumbnail(32, formatManager, thumbnailCache)

{
	playButton.setButtonText("Play");
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	playButton.setEnabled(false);
	playButton.onClick = [this] { playButtonClicked(); };
	addAndMakeVisible(&playButton);

	openButton.setButtonText("Open a File...");
	openButton.setColour(TextButton::buttonColourId, Colours::white);
	openButton.setColour(TextButton::textColourOffId, Colours::black);
	openButton.onClick = [this] { openButtonClicked(); };
	addAndMakeVisible(&openButton);

	stopButton.setButtonText("Stop");
	stopButton.setColour(TextButton::buttonColourId, Colours::red);
	stopButton.setEnabled(false);
	stopButton.onClick = [this] { stopButtonClicked(); };
	addAndMakeVisible(&stopButton);

	processReverbButton.setButtonText("Add Reverb");
	processReverbButton.setColour(TextButton::buttonColourId, Colours::lightskyblue);
	processReverbButton.setColour(TextButton::textColourOffId, Colours::black);
	processReverbButton.setEnabled(false);
	processReverbButton.onClick = [this] {  processReverbButtonClicked(); };
	addAndMakeVisible(&processReverbButton);
	
	saveToFileButton.setButtonText("Save To File...");
	saveToFileButton.setColour(TextButton::buttonColourId, Colours::silver);
	saveToFileButton.setColour(TextButton::textColourOffId, Colours::black);
	saveToFileButton.setEnabled(false);
	saveToFileButton.onClick = [this] { saveToFileButtonClicked();  };
	addAndMakeVisible(&saveToFileButton);

	balanceSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	balanceSlider.setRange(-1.0, 1.0, 0.05);
	balanceSlider.setValue(0.0);
	balanceSlider.setDoubleClickReturnValue(true, 0);
	balanceSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 30);
	addAndMakeVisible(&balanceSlider);

	balanceLabel.setText("Balance", dontSendNotification);
	balanceLabel.setJustificationType(Justification::centred);
	balanceLabel.attachToComponent(&balanceSlider, false);
	addAndMakeVisible(&balanceLabel);

	timeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	timeSlider.setRange(0.0, 5.0, 0.05);
	timeSlider.setTextValueSuffix(" s");
	timeSlider.setValue(1.5);
	timeSlider.setDoubleClickReturnValue(true, 2.5);
	timeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 30);
	addAndMakeVisible(&timeSlider);

	timeLabel.setText("Reverb Time", dontSendNotification);
	timeLabel.setJustificationType(Justification::centred);
	timeLabel.attachToComponent(&timeSlider, false);
	addAndMakeVisible(&timeLabel);

	dampeningSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	dampeningSlider.setRange(0.0, 100.0, 1.0);
	dampeningSlider.setTextValueSuffix(" %");
	dampeningSlider.setValue(75.0);
	dampeningSlider.setDoubleClickReturnValue(true, 50.0);
	dampeningSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 30);
	addAndMakeVisible(&dampeningSlider);

	dampeningLabel.setText("Dampening", dontSendNotification);
	dampeningLabel.setJustificationType(Justification::centred);
	dampeningLabel.attachToComponent(&dampeningSlider, false);
	addAndMakeVisible(&dampeningLabel);

	initSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	initSlider.setRange(0.0, 500.0, 1.0);
	initSlider.setTextValueSuffix(" ms");
	initSlider.setValue(0.0);
	initSlider.setDoubleClickReturnValue(true, 250.0);
	initSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 60, 30);
	addAndMakeVisible(&initSlider);

	initLabel.setText("Initial Delay", dontSendNotification);
	initLabel.setJustificationType(Justification::centred);
	initLabel.attachToComponent(&initSlider, false);
	addAndMakeVisible(&initLabel);

	inGainOffButton.setButtonText(" Off ");
	inGainOffButton.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	inGainOffButton.setToggleState(true, NotificationType::dontSendNotification);
	inGainOffButton.setRadioGroupId(inputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&inGainOffButton);

	inGain5Button.setButtonText(" -   5 dB");
	inGain5Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	inGain5Button.setRadioGroupId(inputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&inGain5Button);

	inGain10Button.setButtonText("- 10 dB");
	inGain10Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	inGain10Button.setRadioGroupId(inputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&inGain10Button);

	inGain20Button.setButtonText("- 20 dB");
	inGain20Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	inGain20Button.setRadioGroupId(inputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&inGain20Button);

	inGain40Button.setButtonText("- 40 dB");
	inGain40Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	inGain40Button.setRadioGroupId(inputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&inGain40Button);

	outGainOffButton.setButtonText(" Off ");
	outGainOffButton.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	outGainOffButton.setToggleState(true, NotificationType::dontSendNotification);
	outGainOffButton.setRadioGroupId(outputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&outGainOffButton);

	outGain5Button.setButtonText(" +   5 dB");
	outGain5Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	outGain5Button.setRadioGroupId(outputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&outGain5Button);

	outGain10Button.setButtonText("+ 10 dB");
	outGain10Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	outGain10Button.setRadioGroupId(outputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&outGain10Button);

	outGain20Button.setButtonText("+ 20 dB");
	outGain20Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	outGain20Button.setRadioGroupId(outputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&outGain20Button);

	outGain40Button.setButtonText("+ 40 dB");
	outGain40Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	outGain40Button.setRadioGroupId(outputGainButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&outGain40Button);

	bitDepth16Button.setButtonText("16 bit");
	bitDepth16Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth16Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth16Button);

	bitDepth24Button.setButtonText("24 bit");
	bitDepth24Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth24Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth24Button);

	bitDepth32Button.setButtonText("32 bit");
	bitDepth32Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::floralwhite);
	bitDepth32Button.setRadioGroupId(bitDepthButtons, NotificationType::dontSendNotification);
	addAndMakeVisible(&bitDepth32Button);

	inputGainLabel.setText("Input Gain", NotificationType::dontSendNotification);
	inputGainLabel.setEditable(false);
	addAndMakeVisible(&inputGainLabel);

	outputGainLabel.setText("Output Gain", NotificationType::dontSendNotification);
	outputGainLabel.setEditable(false);
	addAndMakeVisible(&outputGainLabel);

	clippingLabel.setText("Clipping", NotificationType::dontSendNotification);
	clippingLabel.setEditable(false);
	addAndMakeVisible(&clippingLabel);

	bitDepthLabel.setText("Bit depth: ", NotificationType::dontSendNotification);
	bitDepthLabel.setEditable(false);
	addAndMakeVisible(&bitDepthLabel);

	wetLabel.setText("Wet", NotificationType::dontSendNotification);
	wetLabel.setEditable(false);
	addAndMakeVisible(&wetLabel);

	dryLabel.setText("Dry", NotificationType::dontSendNotification);
	dryLabel.setEditable(false);
	addAndMakeVisible(&dryLabel);

	thumbnail.addChangeListener(this);

    deviceManager.initialise(0, 2, nullptr, true);
	adsetup = deviceManager.getAudioDeviceSetup();
  	adsetup.sampleRate = 44100.0;
    deviceManager.setAudioDeviceSetup(adsetup, true);

	formatManager.registerBasicFormats();

	setSize(1030, 400);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
	bufferToFill.clearActiveBufferRegion();

	if (stopFlag.get() == false)
	{
		bool playbackFinished = false;
		auto numInputChannels = fileBuffer.getNumChannels();
		auto numOutputChannels = bufferToFill.buffer->getNumChannels();
		auto outputSamplesRemaining = bufferToFill.numSamples;
		auto outputSamplesOffset = bufferToFill.startSample;
		while (outputSamplesRemaining > 0)
		{
			auto bufferSamplesRemaining = fileBuffer.getNumSamples() - position;
			auto samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);

			if (samplesThisTime == 0)
			{
				playbackFinished = true;
				break;
			}

			for (auto channel = 0; channel < numOutputChannels; ++channel)
			{
				bufferToFill.buffer->copyFrom(channel,
					outputSamplesOffset,                             
					fileBuffer,                                      
					channel % numInputChannels,                      
					position,                                        
					samplesThisTime);                                
			}
			outputSamplesRemaining -= samplesThisTime;
			outputSamplesOffset += samplesThisTime;
			position += samplesThisTime;    
		}
		if (playbackFinished)
			stopFlag.set(true);
	}
}

void MainComponent::releaseResources()
{
	fileBuffer.setSize (0, 0);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

	Rectangle <int> thumbnailBounds(285, getHeight() - 160, 565, 140);
	if (thumbnail.getNumChannels() == 0)
		paintIfNoFileLoaded(g, thumbnailBounds);
	else
		paintIfFileLoaded(g, thumbnailBounds);

	g.setColour(Colours::silver);
	g.drawEllipse(195, getHeight() - 145, 40, 40, 3.0);

	if (reverb.getIsClipping() == true)
		g.setColour(Colours::red);
	else if (reverb.getIsClipping() == false)
		g.setColour(Colours::forestgreen);

	g.fillEllipse(195, getHeight() - 145, 40, 40);
}

void MainComponent::resized()
{
	openButton.setBounds(10, getHeight() - 60, 80, 40);
	playButton.setBounds(100, getHeight() - 60, 80, 40);
	stopButton.setBounds(190, getHeight() - 60, 80, 40);
	processReverbButton.setBounds(10, getHeight() - 160, 120, 80);
	saveToFileButton.setBounds(getWidth() - 165, getHeight() - 100, 140, 80);

	balanceSlider.setBounds(150, getHeight() - 350, 150, 150);
	timeSlider.setBounds(330, getHeight() - 350, 150, 150);
	dampeningSlider.setBounds(510, getHeight() - 350, 150, 150);
	initSlider.setBounds(690, getHeight() - 350, 150, 150);

	inputGainLabel.setBounds(50, getHeight() - 387, 100, 50);
	outputGainLabel.setBounds(getWidth() - 150, getHeight() - 387, 100, 50);
	clippingLabel.setBounds(185, getHeight() - 115, 100, 50);
	bitDepthLabel.setBounds(getWidth() - 165, getHeight() - 180, 100, 50);
	wetLabel.setBounds(balanceSlider.getScreenX() + 110, balanceSlider.getScreenY() + 80, 50, 50);
	dryLabel.setBounds(balanceSlider.getScreenX() + 10, balanceSlider.getScreenY() + 80, 50, 50);

	inGainOffButton.setBounds(55, getHeight() - 355, 100, 50);
	inGain5Button.setBounds(55, getHeight() - 325, 100, 50);
	inGain10Button.setBounds(55, getHeight() - 295, 100, 50);
	inGain20Button.setBounds(55, getHeight() - 265, 100, 50);
	inGain40Button.setBounds(55, getHeight() - 235, 100, 50);
    
	outGainOffButton.setBounds(getWidth() - 145, getHeight() - 355, 100, 50);
	outGain5Button.setBounds(getWidth() - 145, getHeight() - 325, 100, 50);
	outGain10Button.setBounds(getWidth() - 145, getHeight() - 295, 100, 50);
	outGain20Button.setBounds(getWidth() - 145, getHeight() - 265, 100, 50);
	outGain40Button.setBounds(getWidth() - 145, getHeight() - 235, 100, 50);

	bitDepth16Button.setBounds(getWidth() - 165, getHeight() - 170, 50, 80);
	bitDepth24Button.setBounds(getWidth() - 115, getHeight() - 170, 50, 80);
	bitDepth32Button.setBounds(getWidth() - 65, getHeight() - 170, 50, 80);
}

void MainComponent::openButtonClicked(void)
{
	stopFlag.set(true);

	FileChooser chooser("Select a Wave file shorter than 10 seconds to play...",
		{},
		"*.wav");

	if (chooser.browseForFileToOpen())
	{
		shutdownAudio();
		auto file = chooser.getResult();
		std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

		if (reader.get() != nullptr)
		{
			auto duration = reader->lengthInSamples / reader->sampleRate;	

			if (duration < 10)
			{
				fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
				reader->read(&fileBuffer,
					0,
					(int)reader->lengthInSamples,
					0,
					true,
					true);
				position = 0;
				bitDepth = reader->bitsPerSample;

				if (bitDepth == 24)
					bitDepth24Button.setToggleState(true, NotificationType::dontSendNotification);
				else if (bitDepth == 32)
					bitDepth32Button.setToggleState(true, NotificationType::dontSendNotification);
				else
					bitDepth16Button.setToggleState(true, NotificationType::dontSendNotification);

				playButton.setEnabled(true);
				saveToFileButton.setEnabled(true);
				processReverbButton.setEnabled(true);
				reverb.setInputBuffer(fileBuffer);
				thumbnail.setSource(new FileInputSource(file));
			    setAudioChannels (0, (int) reader->numChannels);
				stopFlag.set(true);
			}
		}
	}
}
void MainComponent::playButtonClicked(void)
{
	changeState(Playing);
}

void MainComponent::stopButtonClicked(void)
{
	stopFlag.set(true);
	changeState(Stopped);
}

void MainComponent::processReverbButtonClicked(void)
{
	reverb.setBalanceCurrentValue(balanceSlider.getValue());
	reverb.setTimeCurrentValue(timeSlider.getValue());
	reverb.setDampeningCurrentValue(dampeningSlider.getValue());
	reverb.setInitCurrentValue(initSlider.getValue());

	if (inGainOffButton.getToggleState())
		reverb.setInputGain(0.0);
	else if (inGain5Button.getToggleState())
		reverb.setInputGain(-5.0);
	else if (inGain10Button.getToggleState())
		reverb.setInputGain(-10.0);
	else if (inGain20Button.getToggleState())
		reverb.setInputGain(-20.0);
	else if (inGain40Button.getToggleState())
		reverb.setInputGain(-40.0);

	if (outGainOffButton.getToggleState())
		reverb.setOutputGain(0.0);
	else if (outGain5Button.getToggleState())
		reverb.setOutputGain(5.0);
	else if (outGain10Button.getToggleState())
		reverb.setOutputGain(10.0);
	else if (outGain20Button.getToggleState())
		reverb.setOutputGain(20.0);
	else if (outGain40Button.getToggleState())
		reverb.setOutputGain(40.0);

	reverb.setupReverbProcessor();
	fileBuffer = reverb.addReverb();

	thumbnail.reset(fileBuffer.getNumChannels(), 44100.0, fileBuffer.getNumSamples());
	thumbnail.addBlock(0, fileBuffer, 0, fileBuffer.getNumSamples());
}

void MainComponent::saveToFileButtonClicked(void)
{
	int outBitDepth;

	if (bitDepth24Button.getToggleState() == true)
		outBitDepth = 24;
	else if (bitDepth32Button.getToggleState() == true)
		outBitDepth = 32;
	else
		outBitDepth = 16;

	FileChooser chooser("Select a WAV File to write to...", {}, "*.wav");

	if (chooser.browseForFileToSave(true))
	{
		auto file = chooser.getResult();

		WavAudioFormat fileFormat;
		OptionalScopedPointer <AudioFormatWriter> fileWriter;
		fileWriter.reset();
		fileWriter.set(fileFormat.createWriterFor(new FileOutputStream(file),
												  44100.0,
												  fileBuffer.getNumChannels(),
												  outBitDepth,
												  {},
												  0), true);
		if (fileWriter != nullptr)
			fileWriter->writeFromAudioSampleBuffer(fileBuffer, 0, fileBuffer.getNumSamples());
	}
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &thumbnail)
		thumbnailChanged();
}

void MainComponent::timerCallback()
{
	if (true == stopFlag.get())
	{
		changeState(Stopped);
		stopTimer();
	}
}

void MainComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;
		switch (state)
		{
		case Playing:
			stopButton.setEnabled(true);
			playButton.setEnabled(false);
			stopFlag.set(false);
			startTimer(10);
			break;

		case Stopped:
			position = 0;
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			break;
		}
	}
}

void MainComponent::thumbnailChanged(void)
{
	repaint();
}

void MainComponent::paintIfNoFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds)
{
	g.setColour(Colours::lightgrey);
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::black);
	g.drawFittedText("No File Loaded", thumbnailBounds, Justification::centred, 1);
}

void MainComponent::paintIfFileLoaded(Graphics& g, const Rectangle <int>& thumbnailBounds)
{
	g.setColour(Colours::lightgrey);
	g.fillRect(thumbnailBounds);
	g.setColour(Colours::darkblue);
	thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0);
}


