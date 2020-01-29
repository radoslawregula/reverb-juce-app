#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>

//==============================================================================
class ReverbProcessor   
{
public:
    ReverbProcessor();
    ~ReverbProcessor();

	bool getIsClipping();

	void setBalanceCurrentValue(float balanceValue);
	void setTimeCurrentValue(float timeValue);
	void setDampeningCurrentValue(float dampeningValue);
	void setInitCurrentValue(float initValue);

	void setInputGain(float inGainChosen);
	void setOutputGain(float outGainChosen);

	void setInputBuffer(AudioBuffer <float>& newFileBuffer);

	void setupReverbProcessor();
	AudioBuffer <float>& addReverb();

private:

//  Coefficients to set up the reverb architecture

	const int numDelayLines;
	float bGain;
	float cGain[12][2];
	float balanceDry;
	const int delayLineLengths[12];
	bool isClipping;
	float inputGain;
	float outputGain;
	int inputBufferSize;
	dsp::Matrix <float> feedbackMatrix;
	OwnedArray <dsp::Matrix <float>> feedbackMatrixRows;
	OwnedArray <dsp::IIR::Filter <float>> dampeningFiltersTab;
	ReferenceCountedArray <dsp::IIR::Coefficients <float>> dampeningFiltersCoeffsTab;
	OptionalScopedPointer <dsp::IIR::Filter <float>> tonalCorrectionFilterPtr;
	dsp::IIR::Coefficients <float>::Ptr tonalCorrectionFilterCoeffsPtr;

//  Input and output buffers 

	AudioBuffer <float> inputBuffer;
	AudioBuffer <float> outputBuffer;

//  Values read from sliders

	float balanceCurrentValue;
	float timeCurrentValue;
	float dampeningCurrentValue;
	float initCurrentValue;

//  Inside class methods

	void setupGainC(float coeff = 1.0);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbProcessor)
};
