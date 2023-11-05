#include "ConstantQTransform.h"

Cqt::TransformationHandler::TransformationHandler(int binsPerOctave) 
	: mCqtBuffer(binsPerOctave)
	, mFft(Fft_Size)
{
	// hard-coded Hann mWindow as of now
	calculateWindow(mWindow, Fft_Size);
	// fft and buffers
	mFftInputBuffer = mFft.valueVector();
	mFftBuffer = mFft.spectrumVector();
	mIfftInputBuffer = mFft.spectrumVector();
	mIfftOutputBuffer = mFft.valueVector();
	std::fill(mIfftOutputBuffer.begin(), mIfftOutputBuffer.end(), 0.);
	mOutputBuffer = mFft.valueVector();
	std::fill(mOutputBuffer.begin(), mOutputBuffer.end(), 0.);
	for (int tone = 0; tone < mCqtBuffer.size(); tone++)
	{
		mCqtBuffer[tone] = 0. + 0.i;
	}

	// kernels
	mKernels.resize(binsPerOctave);
	for (int tone = 0; tone < binsPerOctave; tone++)
	{
		mKernels[tone].array = mFft.spectrumVector();
		mKernels[tone].arrayInverse = mFft.spectrumVector();
	}
}


void Cqt::TransformationHandler::calculateWindow(float* const windowData, const int size)
{
	for (int i = 0; i < size; i++)
	{
		windowData[i] = (1. / 2.) * (1. - std::cos((2. * Pi<float>() * static_cast<float>(i)) / static_cast<float>(size - 1)));
	}
}


void Cqt::TransformationHandler::icqtTransform(const ScheduleElement schedule)
{
	// kernel multiplications
	for (int i = 0; i < Fft_Domain_Size; i++)
	{
		mIfftInputBuffer[i] = 0. + 0.i;
	}
	for (int tone = 0; tone < getBinsPerOctave(); tone++)
	{
		for (size_t i = 0; i < mKernels[tone].maskInv.size(); i++)
		{
			const int idx = mKernels[tone].maskInv[i];
			mIfftInputBuffer[idx] += mCqtBuffer[tone] * mKernels[tone].arrayInverse[idx];
		}
	}
	// ifft
	mFft.inverse(mIfftInputBuffer, mIfftOutputBuffer);
	// scale and mWindow data
	for (int i = 0; i < Fft_Size; i++)
	{
		mIfftOutputBuffer[i] *= mFftScalingFactor;
	}
	for (int i = 0; i < Fft_Size; i++)
	{
		mIfftOutputBuffer[i] *= mInvWindow[i];
	}

	// overlap-add
	std::fill(mOutputBuffer.begin(), mOutputBuffer.end(), 0.);
	//// pull whats left from the previous transform
	mStageOutputBuffer->pullBlock(mOutputBuffer.data(), mStageOutputBuffer->getWriteReadDistance());
	//// add new data
	int count = 0;
	for (int i = schedule.delayOctaveRate; i < (schedule.delayOctaveRate + Fft_Size); i++)
	{
		mOutputBuffer[i] += mIfftOutputBuffer[count];
		count++;
	}
	//// push new data 
	mStageOutputBuffer->pushBlock(mOutputBuffer.data(), Fft_Size + schedule.delayOctaveRate);
}

void Cqt::TransformationHandler::cqtTransform(const ScheduleElement schedule)
{
	// collect the fft input data
	mStageInputBuffer->pullDelayBlock(mFftInputBuffer.data(), static_cast<int>(Fft_Size) + schedule.delayOctaveRate - 1, static_cast<int>(Fft_Size));
	// apply time mWindow
	for (int i = 0; i < Fft_Size; i++)
	{
		mFftInputBuffer[i] *= mWindow[i];
	}
	// fft
	mFft.forward(mFftInputBuffer, mFftBuffer);
	// scale data
	for (int i = 0; i < Fft_Domain_Size; i++)
	{
		mFftBuffer[i] *= mFftScalingFactor;
	}
	// kernel multipications
	for (int tone = 0; tone < getBinsPerOctave(); tone++)
	{
		mCqtBuffer[tone] = 0. + 0.i;
		for (size_t i = 0; i < mKernels[tone].mask.size(); i++)
		{
			const int idx = mKernels[tone].mask[i];
			mCqtBuffer[tone] += mFftBuffer[idx] * mKernels[tone].array[idx];
		}
	}
}


//void Cqt::TransformationHandler::initKernels(const CplxVector* const kernelArray, const CplxVector* const kernelArrayInverse, const std::vector<int>* const kernelMask, const std::vector<int>* const kernelMaskInv)
//{
//	for (int tone = 0; tone < getBinsPerOctave(); tone++)
//	{
//		for (int i = 0; i < Fft_Domain_Size; i++)
//		{
//			mKernels[tone].array[i] = kernelArray[tone].at(i);
//			mKernels[tone].arrayInverse[i] = kernelArrayInverse[tone].at(i);
//		}
//		mKernels[tone].mask.resize(kernelMask[tone].size(), 0);
//		for (size_t i = 0; i < kernelMask[tone].size(); i++)
//		{
//			mKernels[tone].mask[i] = kernelMask[tone][i];
//		}
//		mKernels[tone].maskInv.resize(kernelMaskInv[tone].size(), 0);
//		for (size_t i = 0; i < kernelMaskInv[tone].size(); i++)
//		{
//			mKernels[tone].maskInv[i] = kernelMaskInv[tone][i];
//		}
//	}
//}

Cqt::BufferPtr Cqt::TransformationHandler::getOutputBuffer()
{
	return mStageOutputBuffer;
}

const Cqt::CqtBufferType& Cqt::TransformationHandler::getCqtBuffer()
{
	return mCqtBuffer;
}

void Cqt::TransformationHandler::init(const int hopSize)
{
	calculateInverseWindow(mWindow, mInvWindow, Fft_Size, hopSize);
}

void Cqt::TransformationHandler::initBuffers(BufferPtr inputBuffer, BufferPtr outputBuffer)
{
	mStageInputBuffer = inputBuffer;
	mStageOutputBuffer = outputBuffer;
}


void Cqt::TransformationHandler::initFs(const int blockSize)
{
	mOutputBuffer.resize(Fft_Size + blockSize);
}


void Cqt::TransformationHandler::calculateInverseWindow(float* const windowData, float* const invWindowData, const int size, const int hopSize)
{
	std::vector<float> windowTmp(size, 0.);
	for (int i = 0; i < size; i += hopSize)
	{
		for (int j = 0; j < size; j++)
		{
			windowTmp[(i + j) % size] += windowData[j] * windowData[j];
		}
	}
	for (int i = 0; i < size; i++)
	{
		invWindowData[i] = windowData[i] / windowTmp[i] * std::pow(WindowEnergyLossCompensation, 2);
	}
}

int Cqt::TransformationHandler::getBinsPerOctave() const
{
	return mCqtBuffer.size();
}

Cqt::ConstantQTransform::ConstantQTransform(int binsPerOctave, int octaveNumber) 
	: mFft(Fft_Size)
	, mFftInv(Fft_Size)
	, mFftAllocation(Fft_Size)
{
	mBinNumber = binsPerOctave * octaveNumber;
	mFilterbank.reset(new ResamplingFilterbank(binsPerOctave));

	mFftTmpStorage = mFft.spectrumVector();
	mFftTmpStorageInv = mFft.spectrumVector();
	// configure all the buffer sizes

	mKernelData.resize(binsPerOctave);
	for (int tone = 0; tone < binsPerOctave; tone++)
	{
		mKernelData[tone].mKernelStorageTime = mFft.valueVector();
		mKernelData[tone].mKernelStorageTimeInv = mFft.valueVector();
		mKernelData[tone].mKernelStorage = mFftAllocation.spectrumVector();
		mKernelData[tone].mKernelStorageInv= mFftAllocation.spectrumVector();
	}
	// generate mWindow function
	mWindow.resize(Fft_Size);
	TransformationHandler::calculateWindow(mWindow.data(), Fft_Size);

	// transformation in/out buffers
	mOctaveData.resize(octaveNumber);
	for (int octave = 0; octave < octaveNumber; octave++)
	{
		mOctaveData[octave].mTransformationHandlers.reset(new TransformationHandler(binsPerOctave));
		mOctaveData[octave].mTransformationHandlers->initBuffers(mFilterbank->getStageInputBuffer(octave), mFilterbank->getStageOutputBuffer(octave));
		mOctaveData[octave].mKernelFreqs.resize(binsPerOctave, 0.);
		mOctaveData[octave].mKernelFreqsInv.resize(binsPerOctave, 0.);
		mOctaveData[octave].mSampleCounters = 0;
	}
}

void Cqt::ConstantQTransform::icqt(const ScheduleElement schedule)
{
	mOctaveData[schedule.octave].mTransformationHandlers->icqtTransform(schedule);
}


void Cqt::ConstantQTransform::inputBlock(float* const data, const int blockSize)
{
	// check for new kernels
	if (mNewKernels.load())
	{
		mNewKernels.store(false);
		// calc the windows and give them to handlers
		calculateKernels();
	}
	// process Filterbank and create Schedule
	mFilterbank->inputBlock(data, blockSize);
	// determine cqt positions and schedule them
	mCqtSchedule.clear();
	for (int i = 0; i < blockSize; i++)
	{
		for (int octave = (getOctaveNumber() - 1); octave >= 0; octave--) // starting with lowest pitched octave for historical reasons
		{
			mOctaveData[octave].mSampleCounters ++;
			if (mOctaveData[octave].mSampleCounters >= mOctaveData[octave].mLatencySamples)
			{
				mOctaveData[octave].mSampleCounters = 0;
				const int delayOctaveRate = static_cast<int>(static_cast<float>(blockSize - i - 1) * mOctaveData[octave].mSampleRatesByOriginRate);
				mCqtSchedule.push_back({ i, octave, delayOctaveRate });
			}
		}
	}
}


void Cqt::ConstantQTransform::calculateKernels()
{
	// calculate the time domain kernels
	for (int k = 0; k < getBinsPerOctave(); k++)
	{
		
		const float fk = mOctaveData[0].mKernelFreqs[k];
		const float fkInv = mOctaveData[0].mKernelFreqsInv[k];
		for (int n = 0; n < Fft_Size; n++)
		{
			mKernelData[k].mKernelStorageTime[n] = std::conj((1.f / static_cast<float>(Fft_Size)) * mWindow[n] * std::exp(-1if * 2.f * Pi<float>() * static_cast<float>(n) * (fk / mOctaveData[0].mSampleRates)));
			mKernelData[k].mKernelStorageTimeInv[n] = std::conj((1.f / static_cast<float>(Fft_Size)) * mWindow[n] * std::exp(-1if * 2.f * Pi<float>() * static_cast<float>(n) * (fkInv / mOctaveData[0].mSampleRates)));
		}
	}
	// fft transform kernels and extract necessary (right side of the spectrum) parts
	for (int k = 0; k < getBinsPerOctave(); k++)
	{
		mFft.forward(mKernelData[k].mKernelStorageTime, mFftTmpStorage);
		mFftInv.forward(mKernelData[k].mKernelStorageTimeInv, mFftTmpStorageInv);
		// extract real part
		for (int n = 0; n < Fft_Domain_Size; n++)
		{
			mKernelData[k].mKernelStorage[n] = mFftTmpStorage[n];
			mKernelData[k].mKernelStorageInv[n] = std::conj(mFftTmpStorageInv[n]);
		}
	}
	// mark relevant kernel values
	for (int k = 0; k < getBinsPerOctave(); k++)
	{
		mKernelData[k].mKernelMask.clear();
		mKernelData[k].mKernelMaskInv.clear();
		for (int n = 0; n < Fft_Domain_Size; n++)
		{
			const float kernelAbs = std::abs(mKernelData[k].mKernelStorage[n]);
			if (kernelAbs > KernelThreshold)
			{
				mKernelData[k].mKernelMask.push_back(n);
			}
			const float kernelAbsInv = std::abs(mKernelData[k].mKernelStorageInv[n]);
			if (kernelAbsInv > KernelThreshold)
			{
				mKernelData[k].mKernelMaskInv.push_back(n);
			}
		}
	}
	// pass kernels to handlers
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		auto handlers = mOctaveData[octave].mTransformationHandlers;
		for (int tone = 0; tone < getBinsPerOctave(); tone++)
		{
			for (int i = 0; i < Fft_Domain_Size; i++)
			{
				handlers->mKernels[tone].array[i] = mKernelData[tone].mKernelStorage.at(i);
				handlers->mKernels[tone].arrayInverse[i] = mKernelData[tone].mKernelStorageInv.at(i);
			}

			handlers->mKernels[tone].mask = mKernelData[tone].mKernelMask;
			handlers->mKernels[tone].maskInv = mKernelData[tone].mKernelMaskInv;
		}
	}
}


void Cqt::ConstantQTransform::initKernelFreqs()
{
	/*
	https://en.wikipedia.org/wiki/Piano_key_frequencies
	f(n) = 2^((n-49)/12) * mConcertPitch
	Range in highest octave from n = [100, 111] ~ [8.37 kHz, 15.804 kHz]
	*/
	const float fRef = std::pow(2., ((100. - 49.) / 12.)) * mConcertPitch;
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		for (int tone = 0; tone < getBinsPerOctave(); tone++)
		{
			mOctaveData[octave].mKernelFreqs[tone] = (fRef / std::pow(2., (octave + 1))) * std::pow(2., static_cast<float>(getBinsPerOctave() + tone) / static_cast<float>(getBinsPerOctave()));
			mOctaveData[octave].mKernelFreqsInv[tone] = mOctaveData[octave].mKernelFreqs[tone];
		}
	}
}


void Cqt::ConstantQTransform::init(int hopSize)
{
	initKernelFreqs();

	hopSize = Clip<int>(hopSize, 1, Fft_Size);
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		mOctaveData[octave].mHopSizes = hopSize;
		mOctaveData[octave].mOverlaps = Fft_Size - hopSize;
	}
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		mOctaveData[octave].mTransformationHandlers->init(mOctaveData[octave].mHopSizes);
	}
}

void Cqt::ConstantQTransform::resetKernelFreqs()
{
	initKernelFreqs();
}


size_t Cqt::ConstantQTransform::getLatencySamples(const int octave)
{
	return mOctaveData[octave].mLatencySamples;
}

int Cqt::ConstantQTransform::getHopSize(const int octave)
{
	return mOctaveData[octave].mHopSizes;
}

Cqt::BufferPtr Cqt::ConstantQTransform::getOctaveOutputBuffer(const int octave)
{
	return mOctaveData[octave].mTransformationHandlers->getOutputBuffer();
}

const Cqt::CqtBufferType& Cqt::ConstantQTransform::getOctaveCqtBuffer(const int octave)
{
	return mOctaveData[octave].mTransformationHandlers->getCqtBuffer();
}

std::vector<Cqt::ScheduleElement> Cqt::ConstantQTransform::getCqtSchedule()
{
	return mCqtSchedule;
}

int Cqt::ConstantQTransform::getOctaveNumber() const
{
	return mOctaveData.size();
}

int Cqt::ConstantQTransform::getBinsPerOctave() const
{
	return mKernelData.size();
}

void Cqt::ConstantQTransform::recalculateKernels()
{
	mNewKernels.store(true);
}

float Cqt::ConstantQTransform::getConcertPitch() const
{
	return mConcertPitch;
}

void Cqt::ConstantQTransform::init(std::vector<int> octaveHopSizes)
{
	initKernelFreqs();

	assert(octaveHopSizes.size() == getOctaveNumber());
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		int hopSize = Clip<int>(octaveHopSizes.at(octave), 1, Fft_Size);
		mOctaveData[octave].mHopSizes = hopSize;
		mOctaveData[octave].mOverlaps = Fft_Size - hopSize;
	}
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		mOctaveData[octave].mTransformationHandlers->init(mOctaveData[octave].mHopSizes);
	}
}


void Cqt::ConstantQTransform::initFs(float fs, const int blockSize)
{
	mFilterbank->init(fs, blockSize, blockSize + Fft_Size);
	mFs = mFilterbank->getOriginSamplerate();

	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		// latency per octave
		mOctaveData[octave].mLatencySamples = static_cast<size_t>(mOctaveData[octave].mHopSizes) * static_cast<size_t>(std::pow(2, octave)) * static_cast<size_t>(std::pow(2, mFilterbank->getOriginDownsampling()));
		mOctaveData[octave].mSampleCounters = mOctaveData[octave].mLatencySamples;
		// samplerates

		mOctaveData[octave].mSampleRates = mFs / std::pow(2., octave);
		mOctaveData[octave].mLatencyMs = static_cast<float>(mOctaveData[octave].mHopSizes) / mOctaveData[octave].mSampleRates * 1000.;
		mOctaveData[octave].mSampleRatesByOriginRate = mOctaveData[octave].mSampleRates / mOctaveData[0].mSampleRates;
	}
	for (int octave = 0; octave < getOctaveNumber(); octave++)
	{
		mOctaveData[octave].mTransformationHandlers->initFs(blockSize);
	}
	// calc the windows and give em to handlers
	recalculateKernels();
}


void Cqt::ConstantQTransform::setConcertPitch(float concertPitch)
{
	mConcertPitch = concertPitch;
	initKernelFreqs();
	recalculateKernels();
}


float* Cqt::ConstantQTransform::outputBlock(const int blockSize)
{
	return mFilterbank->outputBlock(blockSize);
}


void Cqt::ConstantQTransform::cqt(const ScheduleElement schedule)
{
	mOctaveData[schedule.octave].mTransformationHandlers->cqtTransform(schedule);
}
