/*
 ==============================================================================

 This file is part of the rt-cqt library. Copyright (C) the rt-cqt developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/*
* Limitations:
* - Fixed Hann mWindow function - for resynthesis a HopSize >= Fft_Size / 2 should be used - HopSize == Fft_Size will produce NaNs
* - No Zero Padding as of now
*/

#include "CircularBuffer.h"
#include "ResamplingFilterbank.h"
#include "Utils.h"
#include <atomic>
#include <memory>

#define SIMD_SZ 1
#include <complex>
#include "pffft.hpp"

namespace Cqt
{

using namespace std::complex_literals;
constexpr int Fft_Size{ 512 };
constexpr int Fft_Domain_Size{ Fft_Size / 2 };
constexpr float KernelThreshold{ 1.e-4f };
constexpr float WindowEnergyLossCompensation{ 1.63f }; // Fixed for Hanning mWindow as of now
constexpr float WindowAmplitudeLossCompensation{ 2.0f }; // Fixed for Hanning mWindow as of now

typedef pffft::AlignedVector<pffft::Types<float>::Complex> CplxVector;
typedef pffft::AlignedVector<float> RealVector;
typedef RealVector TimeDataType;
typedef CplxVector CqtBufferType;


/*
Structure to schedule transformation timings.
*/
struct ScheduleElement
{
	int sample{ 0 }; // Position in actual block
	int octave{ 0 };
	int delayOctaveRate{ 0 }; // Delay of the transformation in samplesin the corresponding octave sample buffer
};

/*
Handles the transformations of the single octaves.
*/

class TransformationHandler
{
	friend class ConstantQTransform;
public:
	TransformationHandler(int binsPerOctave);
	~TransformationHandler() = default;

	void init(const int hopSize);
	void initBuffers(BufferPtr inputBuffer = nullptr, BufferPtr outputBuffer = nullptr);
	void initFs(const int blockSize);

	void cqtTransform(const ScheduleElement schedule);
	void icqtTransform(const ScheduleElement schedule);

	static void calculateWindow(float* const windowData, const int size);
	static void calculateInverseWindow(float* const windowData, float* const invWindowData, const int size, const int hopSize);

	int getBinsPerOctave() const;
	const CqtBufferType& getCqtBuffer();;
	BufferPtr getOutputBuffer();
private:
	float mWindow[Fft_Size];
	float mInvWindow[Fft_Size];

	// kernel storage
	struct Kernel
	{
		CplxVector array; // mB x Fft_Domain_Size
		CplxVector arrayInverse; // mB x Fft_Domain_Size
		std::vector<int> mask; // mB x Fft_Domain_Size
		std::vector<int> maskInv; // mB x Fft_Domain_Size
	};
	std::vector<Kernel> mKernels;

	// scaling 
	const float mFftScalingFactor{ 1.f / std::sqrt(static_cast<float>(Fft_Size)) };
	// pffft
	pffft::Fft<float> mFft;
	RealVector mFftInputBuffer;
	CplxVector mFftBuffer;
	CplxVector mIfftInputBuffer;
	CqtBufferType mCqtBuffer; // mB
	RealVector mOutputBuffer;
	RealVector mIfftOutputBuffer;

	// input / output buffers
	BufferPtr mStageInputBuffer;
	BufferPtr mStageOutputBuffer;
};

/*
Main CQT class
*/

class ConstantQTransform
{
public:
	ConstantQTransform(int binsPerOctave, int octaveNumber);
	~ConstantQTransform() = default;

	void init(int hopSize);
	void init(std::vector<int> octaveHopSizes);
	void initFs(float fs, const int blockSize);

	void setConcertPitch(float concertPitch);
	float getConcertPitch() const;

	void recalculateKernels();;

	void inputBlock(float* const data, const int blockSize);
	float* outputBlock(const int blockSize);
	void cqt(const ScheduleElement schedule);
	void icqt(const ScheduleElement schedule);

	int getBinsPerOctave() const;
	int getOctaveNumber() const;

	std::vector<ScheduleElement> getCqtSchedule();
	const CqtBufferType& getOctaveCqtBuffer(const int octave);
	BufferPtr getOctaveOutputBuffer(const int octave);
	int getHopSize(const int octave);
	size_t getLatencySamples(const int octave);
	void resetKernelFreqs();
protected:
	void initKernelFreqs();
	void calculateKernels();

	float mConcertPitch{ 440. };
	int mBinNumber;

	struct OctaveData
	{
		int mOverlaps;
		float mLatencyMs;
		size_t mLatencySamples;
		int mHopSizes;
		float mSampleRates;
		float mSampleRatesByOriginRate;
		std::shared_ptr<TransformationHandler> mTransformationHandlers;
		size_t mSampleCounters;
		std::vector<float> mKernelFreqs;
		std::vector<float> mKernelFreqsInv;
	};
	std::vector<OctaveData> mOctaveData;

	struct KernelData {
		CplxVector mKernelStorage;
		CplxVector mKernelStorageInv;
		std::vector<int> mKernelMask;
		std::vector<int> mKernelMaskInv;
		CplxVector mKernelStorageTime;
		CplxVector mKernelStorageTimeInv;
	};
	std::vector<KernelData> mKernelData;
	std::atomic<bool> mNewKernels{ true };

	float mFs;
	std::shared_ptr<ResamplingFilterbank> mFilterbank;
	pffft::Fft<std::complex<float>>  mFft;
	pffft::Fft<std::complex<float>>  mFftInv;
	pffft::Fft<float>  mFftAllocation;
	CplxVector mFftTmpStorage;
	CplxVector mFftTmpStorageInv;

	RealVector mWindow;
	std::vector<ScheduleElement> mCqtSchedule;
};

};