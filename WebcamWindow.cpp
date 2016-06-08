/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>

#include <QLabel>
#include <QImage>

#include "WebcamWindow.hxx"

WebcamWindow::WebcamWindow(QWidget *parent):
		QMainWindow(parent),
		m_viewport(new QLabel),
		m_frameMutex(),
		m_frame() {
	setCentralWidget(m_viewport);
}

WebcamWindow::~WebcamWindow() {

}

void WebcamWindow::processFrame(const unsigned char* data, int len) {
	// TODO resolution and format QImage::Format_RGB666
	QImage newFrame(data, 1280, 720, QImage::Format_RGB888);
	m_frameMutex.lock();
	m_frame = newFrame.mirrored(false);
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