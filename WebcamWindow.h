/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef WEBCAM_WINDOW_HXX
#define WEBCAM_WINDOW_HXX

#include <QMainWindow>
#include <QMutex>
#include <QImage>

class QLabel;
class VideoDevice;

class WebcamWindow : public QMainWindow {
	Q_OBJECT

public:
	WebcamWindow(QWidget *parent = 0);
	virtual ~WebcamWindow();

        void processFrame(const unsigned char* data, int len, VideoDevice* device);

private Q_SLOTS:
	void presentFrame();

private:
	QLabel* m_viewport;
	QMutex m_frameMutex;
	QImage m_frame;
};


#endif // WEBCAM_WINDOW_HXX
