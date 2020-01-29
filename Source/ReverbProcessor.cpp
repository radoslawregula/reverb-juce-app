#include "../JuceLibraryCode/JuceHeader.h"
#include "ReverbProcessor.h"
#include <deque>

//==============================================================================
ReverbProcessor::ReverbProcessor() : numDelayLines(12),
                                     bGain(1.0),
									 balanceDry(1.0),
	                                 feedbackMatrix(12, 12),
									 delayLineLengths{ 601, 1399, 1747, 2269, 2707, 3089,
									                    3323, 3571, 3911, 4127, 4639, 4999 },
									 isClipping(false),
									 inputGain(0.0),
	                                 outputGain(0.0),
									 inputBufferSize(0),
									 balanceCurrentValue(0.0),
	                                 timeCurrentValue(1.5),
									 dampeningCurrentValue(75.0),
									 initCurrentValue(0.0)
{
	setupGainC(0.0);

	dsp::Matrix <float> toeplitzVector(12, 1);
	dsp::Matrix <float> circulantMatrix(12, 12);
	dsp::Matrix <float> uVector(12, 1); 
	dsp::Matrix <float> uVectorTransposed(1, 12);
	
	toeplitzVector.clear();
	toeplitzVector(1, 0) = 1.0;
	
	circulantMatrix = circulantMatrix.toeplitz(toeplitzVector, 12);
	for (int cnt = 0; cnt < 11; ++cnt)
		circulantMatrix(cnt, cnt + 1) = 0.0;
	circulantMatrix(0, 11) = 1.0;

	for (int cnt = 0; cnt < 12; ++cnt)
	{
		uVector(cnt, 0) = 1.0;
		uVectorTransposed(0, cnt) = 1.0;
	}
	feedbackMatrix = circulantMatrix - ((uVector * (2.0 / numDelayLines)) * uVectorTransposed);

	for (int cnt = 0; cnt < 12; ++cnt)
		feedbackMatrixRows.add(new dsp::Matrix <float>(1, 12, feedbackMatrix.getRawDataPointer() + cnt * 12));

	inputBuffer.clear();
	outputBuffer.clear();
	dampeningFiltersTab.clear();
	dampeningFiltersCoeffsTab.clear();
}

ReverbProcessor::~ReverbProcessor()
{
}

bool ReverbProcessor::getIsClipping()
{
	return isClipping;
}

void ReverbProcessor::setBalanceCurrentValue (float balanceValue)
{
	balanceCurrentValue = balanceValue;
}

void ReverbProcessor::setTimeCurrentValue (float timeValue)
{
	timeCurrentValue = timeValue;
}

void ReverbProcessor::setDampeningCurrentValue (float dampeningValue)
{
	dampeningCurrentValue = dampeningValue;
}

void ReverbProcessor::setInitCurrentValue (float initValue)
{
	initCurrentValue = initValue;
}

void ReverbProcessor::setInputGain(float inGainChosen)
{
	inputGain = inGainChosen;
}

void ReverbProcessor::setOutputGain(float outGainChosen)
{
	outputGain = outGainChosen;
}

void ReverbProcessor::setInputBuffer(AudioBuffer <float>& newFileBuffer)
{
	inputBuffer.makeCopyOf(newFileBuffer);
	inputBufferSize = inputBuffer.getNumSamples();
	isClipping = false;
}

void ReverbProcessor::setupReverbProcessor()
{

	outputBuffer.setSize(inputBuffer.getNumChannels(),
			inputBufferSize
			+ int(44100.0 * timeCurrentValue)
		    + int(std::ceil(44.1 * initCurrentValue))
			+ 11025,
			false,
			true,
			false);
	inputBuffer.setSize(inputBuffer.getNumChannels(),
			outputBuffer.getNumSamples(),
			true,
			true,
			false);

	bGain = Decibels::decibelsToGain(inputGain);
	outputGain = Decibels::decibelsToGain(outputGain);

	if (balanceCurrentValue <= 0.0)
	{
		setupGainC(std::pow(balanceCurrentValue + 1.0, 4));
		balanceDry = 1.0;
	}
	else if (balanceCurrentValue > 0.0)
	{
		setupGainC();
		balanceDry = std::pow(1.0 - balanceCurrentValue, 4);
	}

	dampeningFiltersTab.clear();
	dampeningFiltersCoeffsTab.clear();
	tonalCorrectionFilterPtr.clear();
	tonalCorrectionFilterCoeffsPtr.reset();

	float minAlphaConst = (4.0 * timeCurrentValue) / (-3.0 * 4999.0 * (1.0 / 44100.0) * std::log(10.0));
	float minAlpha = std::sqrt(1.0 / (1.0 - minAlphaConst));
	float alpha = minAlpha + ((100.0 - dampeningCurrentValue) * ((1.0 - minAlpha) / 100.0));

	float gCoeff = 0.0;
	float pCoeff = 0.0;
	float constElement1 = std::log(10.0) / 4.0;
	float constElement2 = 1.0 - (1.0 / std::pow(alpha, 2));

	for (int cnt = 0; cnt < 12; ++cnt)
	{
		gCoeff = std::pow(10.0, (-3.0 * delayLineLengths[cnt] * (1.0 / 44100.0)) / timeCurrentValue);
		pCoeff = constElement1 * constElement2 * std::log10(gCoeff);
		dampeningFiltersCoeffsTab.add(new dsp::IIR::Coefficients <float>(gCoeff * (1.0 - pCoeff), 0.0, 1.0, -pCoeff));
		dampeningFiltersTab.add(new dsp::IIR::Filter <float>(dampeningFiltersCoeffsTab.getLast()));
	}

	float beta = (1.0 - alpha) / (1.0 + alpha);
	tonalCorrectionFilterCoeffsPtr = new dsp::IIR::Coefficients <float>(1.0, -beta, 1.0 - beta, 0.0);
	tonalCorrectionFilterPtr.set(new dsp::IIR::Filter <float>(tonalCorrectionFilterCoeffsPtr.get()), true);

	isClipping = false;
}

AudioBuffer <float>& ReverbProcessor::addReverb()
{
	ScopedNoDenormals noDenormals;

	int initDelayLineLength = 1;

	if (initCurrentValue != 0.0)
		initDelayLineLength = int(std::round(initCurrentValue * 44.1));

	std::deque <float> initDelayLine(initDelayLineLength, 0.0);
	OwnedArray <std::deque <float>> delayLines;
	dsp::Matrix <float> tempMatrix(12, 1);
	float outSample = 0.0;
	float currentSample = 0.0;
	dsp::Matrix <float> currentMatrixProduct(1, 1);

	for (int cnt = 0; cnt < 12; ++cnt)
		delayLines.add(new std::deque <float>(4999, 0.0));

	for (auto channel = 0; channel < inputBuffer.getNumChannels(); ++channel)
	{
		float* outputWrite = outputBuffer.getWritePointer(channel);
		const float* inputRead = inputBuffer.getReadPointer(channel);

		for (auto sample = 0; sample < outputBuffer.getNumSamples(); ++sample)
		{
			outSample = 0.0;
			for (int cnt = 0; cnt < 12; ++cnt)
			{
				tempMatrix(cnt, 0) = (*delayLines[cnt])[delayLineLengths[cnt] - 1];
				outSample += tempMatrix(cnt, 0) * cGain[cnt][channel];

				currentMatrixProduct = *feedbackMatrixRows[cnt] * tempMatrix;
				currentSample = (inputRead[sample] * bGain) + *(currentMatrixProduct.getRawDataPointer());
				currentSample = dampeningFiltersTab[cnt]->processSample(currentSample);
				delayLines[cnt]->push_front(currentSample);
				delayLines[cnt]->pop_back();
			}

			outSample = tonalCorrectionFilterPtr.get()->processSample(outSample);
			initDelayLine.push_front(outSample);
			initDelayLine.pop_back();
			outputWrite[sample] = ((inputRead[sample] * bGain * balanceDry)
				+ initDelayLine.back()) * outputGain;

			if (isClipping == false)
			{
				if (outputWrite[sample] > 1.0 || outputWrite[sample] < -1.0)
					isClipping = true;
			}
		}

		for (int cnt = 0; cnt < 12; ++cnt)
			dampeningFiltersTab[cnt]->reset();
		tonalCorrectionFilterPtr->reset();
	}

	auto a = outputBuffer.getMagnitude(0, outputBuffer.getNumSamples());
	return outputBuffer;
}

void ReverbProcessor::setupGainC(float coeff)
{
	unsigned short flag = 1;
	for (int cnt1 = 0; cnt1 < 12; ++cnt1)
	{
		for (int cnt2 = 0; cnt2 < 2; ++cnt2)
		{
			if (flag % 3 != 0 && flag != 7 && flag != 8)
				cGain[cnt1][cnt2] = 1.0 * coeff;
			else
				cGain[cnt1][cnt2] = -1.0 * coeff;
			++flag;
			if (flag == 9) flag = 1;
		}
	}
}


