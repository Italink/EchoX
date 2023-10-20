#include "QAudioAnalyseView.h"

const int gHistogramBarWidth = 20;
const int gSpectrogramHeight = 250;

class QAudioHistogram: public QWidget{
public:
	QAudioHistogram() {
		setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
	}
	void updateData(const QVector<float>& data) {
		mData = data;
		setFixedHeight(data.size() * gHistogramBarWidth);
	}
protected:
	void paintEvent(QPaintEvent* e) override {

	}
private:
	QVector<float> mData;
};

class QAudioSpectrogram : public QWidget {
public:
	QAudioSpectrogram() {
		setFixedHeight(gSpectrogramHeight);
	}
	void updateData(const QVector<float>& data) {
		mData = data;
	}
protected:
	void paintEvent(QPaintEvent* e) override {

	}
private:
	QVector<float> mData;
};

QAudioAnalyseView::QAudioAnalyseView()
{

}
