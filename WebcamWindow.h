/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef WEBCAM_WINDOW_HXX
#define WEBCAM_WINDOW_HXX

#include <QMainWindow>
#include <QMutex>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>

class QLabel;
class VideoDevice;
class VideoCapture;

class WebcamWindow : public QMainWindow {
	Q_OBJECT

public:
	static WebcamWindow& getInstance() {
		static WebcamWindow instance;
		return instance;
	}

	void processFrame(const unsigned char* data, int len, VideoDevice* device);

private:
	WebcamWindow(QWidget *parent = nullptr);
	virtual ~WebcamWindow();
	WebcamWindow(const WebcamWindow& other) = delete;
	WebcamWindow& operator=(const WebcamWindow& other) = delete;

private Q_SLOTS:
	void presentFrame();

private:
	QLabel* m_viewport;
	QMutex m_frameMutex;
	QImage m_frame;
	QVBoxLayout* m_controlLayout;
	QGroupBox* m_controlGroup;
	QHBoxLayout* m_windowLayout;
	QGroupBox* m_windowGroup;
	QPushButton* m_startButton;
	QPushButton* m_stopButton;
	QComboBox* m_devices;
	QComboBox* m_resolutions;
	QSplitter* m_vsplitter;
	VideoCapture* m_videoCapture;
};


#endif // WEBCAM_WINDOW_HXX
