#include "QAudioAnalyseView.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QQueue>
#include <QLabel>

const int gOscillogramQueueCapacity = 50;
const int gDiagramHeight = 150;

class QAudioOscillogram : public QWidget {
public:
	QAudioOscillogram(const QString& name) {
		setObjectName(name);
		mQueue.resize(gOscillogramQueueCapacity);
	}

	void addData(float data) {
		mQueue.dequeue();
		mQueue.enqueue(data);
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		int fontHeight = painter.fontMetrics().height();
		int maxHeight = height() - fontHeight;
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		float offset = width() / (float)mQueue.size();

		path.moveTo(0, maxHeight);
		for (int i = 0; i < mQueue.size(); i++) {
			path.lineTo(i * offset, qBound(0.0f, (1 - mQueue[i]), 1.0f) * maxHeight);
		}
		path.lineTo(width(), maxHeight);

		QPen pen(QColor(34, 147, 66), 1);
		painter.setPen(pen);
		painter.drawPath(path);

		path.closeSubpath();

		QLinearGradient linear;
		linear.setStart(0, maxHeight);
		linear.setFinalStop(0, 0);
		linear.setColorAt(0, QColor(30, 207, 159, 50));
		linear.setColorAt(0.2, QColor(30, 207, 159));
		painter.fillPath(path, linear);

		painter.setPen(QPen(QColor(30, 30, 30), 1));
		QRect rect(0, 0, width(), fontHeight);
		rect.moveBottom(height());
		painter.drawText(rect, Qt::AlignCenter, objectName());
	}
protected:
	QQueue<float> mQueue;
};

class QHighFrequencyContentOscillogram : public QWidget {
public:
	QHighFrequencyContentOscillogram(const QString& name) {
		setObjectName(name);
		setFixedHeight(40);
	}

	void addFreq(float data) {
		mSrcFreq = mDstFreq;
		mDstFreq = data;
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		int fontHeight = painter.fontMetrics().height();
		int maxHeight = height() - fontHeight;
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;

		QPoint src(mSrcFreq * width(), maxHeight / 2);
		QPoint dst(mDstFreq * width(), maxHeight / 2);

		QLinearGradient linear;
		linear.setStart(src);
		linear.setFinalStop(dst);
		linear.setColorAt(0, QColor(30, 207, 159, 50));
		linear.setColorAt(1, QColor(30, 207, 159));

		QPen pen(linear, 10);
		painter.setPen(pen);
		painter.fillRect(QRect(0,0,width(),maxHeight), QColor(30, 207, 159, 20));
		painter.drawLine(src, dst);

		painter.setPen(QPen(QColor(30, 30, 30), 1));
		QRect rect(0, 0, width(), fontHeight);
		rect.moveBottom(height());
		painter.drawText(rect, Qt::AlignCenter, objectName());
	}
protected:
	float mSrcFreq = 0;
	float mDstFreq = 0;
};

class QAudioSpectrogram : public QWidget {
public:
	QAudioSpectrogram(const QString& name) {
		setObjectName(name);
	}
	void updateData(const QVector<float>& data) {
		mData = data;
	}
protected:
	void paintEvent(QPaintEvent* e) override {
		QPainter painter(this);
		float offset = width() / (float)mData.size();
		int fontHeight = painter.fontMetrics().height();
		int maxHeight = height() - fontHeight - offset;

		painter.translate(0, height() - fontHeight);
		painter.scale(1, -1);


		QLinearGradient linear;
		linear.setStart(0, 0);
		linear.setFinalStop(0, height());
		linear.setColorAt(0, QColor(30, 207, 159, 50));
		linear.setColorAt(0.2, QColor(30, 207, 159));

		QPen pen(QColor(30, 207, 159), offset);
		painter.setPen(pen);
		for (int i = 0; i < mData.size(); i++) {
			painter.drawLine(QPointF(i * offset, offset/2), QPointF(i * offset, qBound(0.0f, mData[i], 1.0f) * maxHeight + offset/2));
		}		
		painter.resetTransform();

		painter.setPen(QPen(QColor(30, 30, 30),1));
		QRect rect(0, 0, width(), fontHeight);
		rect.moveBottom(height());
		painter.drawText(rect, Qt::AlignCenter, objectName());
	}
private:
	QVector<float> mData;
};

QAudioAnalyseView::QAudioAnalyseView()
	: mDebugInformant(QAudioAnalyseManager::Get().createInformant())
	, mRootMeanSquare(new QAudioOscillogram("Root Mean Square"))
	, mPeakEnergy(new QAudioOscillogram("Peak Energy"))
	, mEnergyDifference(new QAudioOscillogram("Energy Difference"))
	, mSpectralDifference(new QAudioOscillogram("Spectral Difference"))
	, mMagnitudeSpectrum(new QAudioSpectrogram("Magnitude Spectrum"))
	, mConstantQTransformSpectrum(new QAudioSpectrogram("Constant-Q Transform Spectrum"))
	, mMelFrequencySpectrum(new QAudioSpectrogram("Mel Frequency Spectrum"))
	, mHighFrequencyContent(new QHighFrequencyContentOscillogram("High Frequency Content"))
{
	mDebugInformant->setCqtSetting(16, 128, 1);

	QPalette palette(this->palette());
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);

	QGridLayout* gLayout = new QGridLayout(this);
	gLayout->addWidget(new QLabel("Time Domain Features"), 0, 0, 2, 1);
	gLayout->addWidget(mRootMeanSquare, 0, 1, 1, 2);
	gLayout->addWidget(mPeakEnergy, 0, 3, 1, 2);
	gLayout->addWidget(mEnergyDifference, 1, 1, 1, 2);
	gLayout->addWidget(mSpectralDifference, 1, 3, 1, 2);

	gLayout->addWidget(new QLabel("Frequency Domain Features"), 2, 0, 3, 1);

	gLayout->addWidget(mMagnitudeSpectrum, 2, 1, 1, 4);

	gLayout->addWidget(mHighFrequencyContent, 3, 1, 1, 4);

	gLayout->addWidget(mConstantQTransformSpectrum, 4, 1, 1, 3);
	gLayout->addWidget(mMelFrequencySpectrum, 4, 4, 1, 1);

	//gLayout->setColumnStretch(0, 3);
	//gLayout->setColumnStretch(1, 4);
	//gLayout->setColumnStretch(2, 4);
	//gLayout->setColumnStretch(3, 4);
	//gLayout->setColumnStretch(4, 4);

	gLayout->setRowStretch(0, 80);
	gLayout->setRowStretch(1, 80);
	gLayout->setRowStretch(2, 80);
	gLayout->setRowStretch(3, 80);
	gLayout->setRowStretch(4, 80);
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
	auto cqtSchedule = mDebugInformant->getCqtSchedule();
	if (!cqtSchedule.empty()) {
		Cqt::ScheduleElement schedule;
		mConstantQTransformSpectrum->updateData(mDebugInformant->getCqtMagnitudeSpectrum(schedule));
	}
	mMelFrequencySpectrum->updateData(mDebugInformant->getMelFrequencySpectrum());
	mHighFrequencyContent->addFreq(mDebugInformant->getHighFrequencyContent() / mDebugInformant->getSampleRate());

	update();
}
