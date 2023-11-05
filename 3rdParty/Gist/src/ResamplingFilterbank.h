/*
 ==============================================================================

 This file is part of the rt-cqt library. Copyright (C) the rt-cqt developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

#include <vector>
#include "Resampling.h"
#include "CircularBuffer.h"

/*
This class handles multirate resampling of the input data. First to 44.1/48 kHz and from there to the rates of the different stages.
The incoming blocks are downsampled by 2 until the remaining sample number cannot be divided by 2 anymore. From there sample-based downsampling is used. 
This method should cover all possible input block sizes.

Note: at the moment only (samplerates > 44.1 kHz) and (((fsInt % 44100) == 0) || ((fsInt % 48000) == 0)) are supported. 
*/

namespace Cqt
{

typedef CircularBuffer<float>* BufferPtr;

constexpr float FilterTransitionBandwidth{ 0.1 };
constexpr unsigned AllpassNumber{ 3 };

class ResamplingFilterbank
{
public:
	ResamplingFilterbank(int stageNumber);
	~ResamplingFilterbank() = default;

	void init(const float samplerate, const int blockSize, const int bufferSize);

	void inputBlock(float* const data, const int blockSize);
	float* outputBlock(const int blockSize);

	float getOriginSamplerate();
	int getOriginBlockSize();
	int getStageNumber() const;
	BufferPtr getStageInputBuffer(const int stage);
	BufferPtr getStageOutputBuffer(const int stage);
	int getOriginDownsampling();
private:
	ResamplingHandler<float, AllpassNumber> mInputResamplingHandler;

	std::vector<HalfBandLowpass<float, AllpassNumber>> mDownsamplingFilters;
	std::vector<HalfBandLowpass<float, AllpassNumber>> mUpsamplingFilters;
	std::vector<CircularBuffer<float>> mStageInputBuffers;
	std::vector<CircularBuffer<float>> mStageOutputBuffers;

	float mOriginSamplerate;
	int mOriginBlockSize;
	int mOriginDownsampling;

	std::vector<float> mStageSamplerates;
	std::vector<int> mDownsamplingBlockSizes;
	std::vector<int> mUpsamplingBlockSizes;

	// block based
	int mBlockFilterNumber;

	// sample based
	int mSampleInputSize{ 0 };
	int mSampleFilterNumber;
	std::vector<std::vector<bool>> mIsSample;
	std::vector<float> mUpsamplingSampleBuffer;
	std::vector<float> mUpsamplingSampleOutputBuffer;

	// handling of input / output buffering
	CircularBuffer<float> mInputBuffer;
	CircularBuffer<float> mOutputBuffer;
	std::vector<float> mInputData;
	std::vector<float> mOutputData;
	size_t mInputDataCounter{ 0 };
	size_t mOutputDataCounter{ 0 };
	int mExpectedBlockSize{ 0 };
};

}