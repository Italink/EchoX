#include "QAudioAnalyseView.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QQueue>
#include <QLabel>
#include "Widgets/QElideLabel.h"

const int gOscillogramQueueCapacity = 50;
const int gDiagramHeight = 150;

class QAudioOscillogram: public QWidget{
public:
	QAudioOscillogram(const QString& name) {
		setObjectName(name);
		setFixedHeight(gDiagramHeight);
		mQueue.resize(gOscillogramQueueCapacity);
	}

	void addData(float data) {
		mQueue.dequeue();
		mQueue.enqueue(data);
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		float offset = width() / (float)mQueue.size();

		path.moveTo(0, height());
		for (int i = 0; i < mQueue.size(); i++) {
			path.lineTo(i * offset, qBound(0.0f, (1 - mQueue[i]), 1.0f) * height());
		}
		path.lineTo(width(), height());

		QPen pen(QColor(34, 147, 66), 1);
		painter.setPen(pen);
		painter.drawPath(path);

		path.closeSubpath();

		QLinearGradient linear;
		linear.setStart(0, height());
		linear.setFinalStop(0, 0);
		linear.setColorAt(0, QColor(30,207,159, 50));
		linear.setColorAt(0.2, QColor(30,207,159));
		painter.fillPath(path, linear);
	}
protected:
	QQueue<float> mQueue;
};

class QHighFrequencyContentOscillogram : public QWidget {
public:
	QHighFrequencyContentOscillogram() {
		setFixedHeight(10);
	}

	void addFreq(float data) {
		mSrcFreq = mDstFreq;
		mDstFreq = data;
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;

		QPoint src(mSrcFreq * width(), height() / 2);
		QPoint dst(mDstFreq * width(), height() / 2);

		QLinearGradient linear;
		linear.setStart(src);
		linear.setFinalStop(dst);
		linear.setColorAt(0, QColor(30,207,159, 50));
		linear.setColorAt(1, QColor(30,207,159));

		QPen pen(linear,10);
		painter.setPen(pen);
		painter.fillRect(rect(), QColor(30,207,159, 20));
		painter.drawLine(src, dst);
	}
protected:
	float mSrcFreq = 0;
	float mDstFreq = 0;
};

class QAudioSpectrogram : public QWidget {
public:
	QAudioSpectrogram(const QString& name) {
		setObjectName(name);
		setFixedHeight(gDiagramHeight);
	}
	void updateData(const std::vector<float>& data) {
		mData = data;
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		painter.translate(0, height());
		painter.scale(1, -1);      

		float offset = width() / (float)mData.size();

		QLinearGradient linear;
		linear.setStart(0, 0);
		linear.setFinalStop(0, height());
		linear.setColorAt(0, QColor(30,207,159, 50));
		linear.setColorAt(0.2, QColor(30,207,159));

		QPen pen(QColor(30,207,159), offset);
		painter.setPen(pen);
		for (int i = 0; i < mData.size(); i++) {
			painter.drawLine(QPointF(i * offset, 0), QPointF(i * offset, qBound(0.0f, mData[i]* 0.85, 1.0f) * height()));
		}
	}
private:
	std::vector<float> mData;
};

QAudioAnalyseView::QAudioAnalyseView()
	: mDebugInformant(QAudioAnalyseManager::Get().createInformant())
	, mRootMeanSquare(new QAudioOscillogram("RootMeanSquare"))
	, mPeakEnergy(new QAudioOscillogram("PeakEnergy"))
	, mEnergyDifference(new QAudioOscillogram("EnergyDifference"))
	, mSpectralDifference(new QAudioOscillogram("SpectralDifference"))
	, mMagnitudeSpectrum(new QAudioSpectrogram("MagnitudeSpectrum"))
	, mMelFrequencySpectrum(new QAudioSpectrogram("MelFrequencySpectrum"))
	, mHighFrequencyContent(new QHighFrequencyContentOscillogram())

{
	QPalette palette(this->palette());
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);

	QGridLayout* gLayout = new QGridLayout(this);

	gLayout->addWidget(new QElideLabel("Time Domain Features"), 0, 0, 2, 1);
	gLayout->addWidget(mRootMeanSquare, 0, 1, 1, 2);
	gLayout->addWidget(new QLabel("Root Mean Square"), 1, 1, 1, 2, Qt::AlignCenter);
	gLayout->addWidget(mPeakEnergy, 0, 3, 1, 2);
	gLayout->addWidget(new QLabel("Peak Energy"), 1, 3, 1, 2, Qt::AlignCenter);
	gLayout->addWidget(mEnergyDifference, 2, 1, 1, 2);
	gLayout->addWidget(new QLabel("Energy Difference"), 3, 1, 1, 2, Qt::AlignCenter);
	gLayout->addWidget(mSpectralDifference, 2, 3, 1, 2);
	gLayout->addWidget(new QLabel("Spectral Difference"), 3, 3, 1, 2, Qt::AlignCenter);

	gLayout->addWidget(new QElideLabel("Spectrum"), 4, 0, 2 ,1);
	gLayout->addWidget(mMagnitudeSpectrum, 4, 1, 1, 3);
	gLayout->addWidget(new QLabel("Magnitude Spectrum"), 5, 1, 1, 3, Qt::AlignCenter);
	gLayout->addWidget(mMelFrequencySpectrum, 4, 4, 1, 1);
	gLayout->addWidget(new QLabel("Mel Frequency Spectrum"), 5, 4, 1, 1, Qt::AlignCenter);
	gLayout->addWidget(mHighFrequencyContent, 6, 1, 1, 3);
	gLayout->addWidget(new QLabel("High Frequency Content"), 7, 1, 1, 3, Qt::AlignCenter);

	gLayout->setColumnStretch(0, 3);
	gLayout->setColumnStretch(1, 4);
	gLayout->setColumnStretch(2, 4);
	gLayout->setColumnStretch(3, 4);
	gLayout->setColumnStretch(4, 4);

	gLayout->setRowStretch(0, 5);
	gLayout->setRowStretch(1, 1);
	gLayout->setRowStretch(2, 5);
	gLayout->setRowStretch(3, 1);
	gLayout->setRowStretch(4, 5);
	gLayout->setRowStretch(5, 1);
	gLayout->setRowStretch(6, 5);
	gLayout->setRowStretch(7, 1);

	connect(&mTimer, &QTimer::timeout, this, &QAudioAnalyseView::updateView);
	mTimer.setInterval(30);
	mTimer.start();
}

void QAudioAnalyseView::updateView()
{
	mRootMeanSquare->addData(mDebugInformant->getRootMeanSquare());
	mPeakEnergy->addData(mDebugInformant->getPeakEnergy());

	mEnergyDifference->addData(mDebugInformant->getEnergyDifference());
	mSpectralDifference->addData(mDebugInformant->getSpectralDifference() / 200);

	mMagnitudeSpectrum->updateData(mDebugInformant->getMagnitudeSpectrum());
	mMelFrequencySpectrum->updateData(mDebugInformant->getMelFrequencySpectrum());
	mHighFrequencyContent->addFreq(mDebugInformant->getHighFrequencyContent() / mDebugInformant->getSampleRate());

	update();
}
