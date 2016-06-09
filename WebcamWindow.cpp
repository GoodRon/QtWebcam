/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>

#include <QLabel>
#include <QImage>

#include "WebcamWindow.h"
#include "VideoDevice.h"
#include "VideoCapture.h"

void callback(unsigned char* data, int len, VideoDevice* device) {
	WebcamWindow::getInstance().processFrame(data, len, device);
}

WebcamWindow::WebcamWindow(QWidget *parent):
		QMainWindow(parent),
		m_viewport(new QLabel),
		m_frameMutex(),
		m_frame(),
		m_controlLayout(new QVBoxLayout),
		m_controlGroup(new QGroupBox),
		m_windowLayout(new QHBoxLayout),
		m_windowGroup(new QGroupBox),
		m_startButton(new QPushButton),
		m_stopButton(new QPushButton),
		m_devices(new QComboBox),
		m_resolutions(new QComboBox),
		m_vsplitter(new QSplitter),
		m_videoCapture(nullptr) {
	setWindowTitle("QtWebcam");

	m_controlLayout->addWidget(m_devices);
	m_controlLayout->addWidget(m_resolutions);
	m_controlLayout->addWidget(m_vsplitter);
	m_controlLayout->addWidget(m_startButton);
	m_controlLayout->addWidget(m_stopButton);
	m_controlGroup->setLayout(m_controlLayout);
	m_controlGroup->setMinimumWidth(200);
	m_controlGroup->setMaximumWidth(200);

	m_viewport->setMinimumSize(320, 240);
	m_windowLayout->addWidget(m_viewport);
	m_windowLayout->addWidget(m_controlGroup);
	m_windowGroup->setLayout(m_windowLayout);
	setCentralWidget(m_windowGroup);

	m_videoCapture = new VideoCapture(callback);
	m_videoCapture->startCapture();

	auto devicesNames = m_videoCapture->getDevicesNames();
	for (auto& deviceName: devicesNames) {
		m_devices->addItem(deviceName);
	}

	auto deviceResolutions = m_videoCapture->getActiveDeviceResolutions();
	for (auto& deviceResolution: deviceResolutions) {
		m_resolutions->addItem(deviceResolution);
	}
}

WebcamWindow::~WebcamWindow() {
	delete m_videoCapture;
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
