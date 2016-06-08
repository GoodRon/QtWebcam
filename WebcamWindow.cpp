/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>

#include <QLabel>
#include <QImage>

#include "WebcamWindow.h"
#include "VideoDevice.h"

WebcamWindow::WebcamWindow(QWidget *parent):
		QMainWindow(parent),
		m_viewport(new QLabel),
		m_frameMutex(),
		m_frame() {
	setCentralWidget(m_viewport);
}

WebcamWindow::~WebcamWindow() {

}

QImage::Format getImageFormat(GUID uid) {
	if (uid == MEDIASUBTYPE_ARGB32) {
		return QImage::Format_ARGB32;
	}
	if (uid == MEDIASUBTYPE_RGB32) {
		return QImage::Format_RGB32;
	}
	if (uid == MEDIASUBTYPE_RGB24) {
		return QImage::Format_RGB888;
	}
	if (uid == MEDIASUBTYPE_RGB555) {
		return QImage::Format_RGB555;
	}
    return QImage::Format_Invalid;
}

void WebcamWindow::processFrame(const unsigned char* data, int len, VideoDevice* device) {
        if (!device) {
            return;
        }

        auto format = getImageFormat(device->getCurrentProperties().pixelFormat);
        QImage newFrame(data, device->getCurrentProperties().width,
                        device->getCurrentProperties().height, format);
	m_frameMutex.lock();
        m_frame = newFrame.mirrored(device->getCurrentProperties().isFlippedHorizontal,
                                    !device->getCurrentProperties().isFlippedVertical);
        // TODO check format
	m_frame = m_frame.rgbSwapped();
	m_frameMutex.unlock();

	QMetaObject::invokeMethod(this, "presentFrame", Qt::QueuedConnection);
}

void WebcamWindow::presentFrame() {
	m_frameMutex.lock();
	m_viewport->setPixmap(QPixmap::fromImage(m_frame));
	m_frameMutex.unlock();
	m_viewport->repaint();
}
