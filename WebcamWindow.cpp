/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>

#include <QLabel>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>
#include <QString>

#include "WebcamWindow.h"
#include "VideoDevice.h"
#include "VideoCapture.h"
#include "ImageFormats.h"

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
    m_startButton(new QPushButton("Turn On")),
    m_stopButton(new QPushButton("Turn Off")),
    m_devices(new QComboBox),
    m_resolutions(new QComboBox),
    m_vsplitter(new QSplitter),
    m_videoCapture(nullptr),
    m_isCapturing(false) {
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
    m_windowLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_windowGroup->setLayout(m_windowLayout);
    setCentralWidget(m_windowGroup);

    m_videoCapture = new VideoCapture(callback);

    auto devicesNames = m_videoCapture->getDevicesNames();
    for (auto& deviceName: devicesNames) {
        QString name = QString::fromWCharArray(deviceName.c_str());
        m_devices->addItem(name);
    }

    auto deviceResolutions = m_videoCapture->getActiveDeviceResolutions();
    for (auto& deviceResolution: deviceResolutions) {
        QString resolution = QString::fromStdString(deviceResolution);
        m_resolutions->addItem(resolution);
    }

    connect(m_resolutions, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, static_cast<void(WebcamWindow::*)(int)>(&WebcamWindow::changeResolution));
    connect(m_devices, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, static_cast<void(WebcamWindow::*)(int)>(&WebcamWindow::changeDevice));
    connect(m_startButton, &QPushButton::released, this, &WebcamWindow::startCapture);
    connect(m_stopButton, &QPushButton::released, this, &WebcamWindow::stopCapture);
}

WebcamWindow::~WebcamWindow() {
    m_videoCapture->stopCapture();
    delete m_videoCapture;
}

void WebcamWindow::processFrame(const unsigned char* data, int len, VideoDevice* device) {
    if (!device || data == nullptr || len <= 0) {
        return;
    }

    QImageMaker makeQImage;
    for (auto& formatRow: ImageFormatTable) {
        if (formatRow.directshowFormat == device->getCurrentProperties().pixelFormat) {
            makeQImage = formatRow.makeQImage;
            break;
        }
    }

    QImage newFrame(makeQImage(data, len, device->getCurrentProperties().width,
                               device->getCurrentProperties().height));
    m_frameMutex.lock();
    m_frame = newFrame.mirrored(device->getCurrentProperties().isFlippedHorizontal,
                                !device->getCurrentProperties().isFlippedVertical);
    m_frameMutex.unlock();

    QMetaObject::invokeMethod(this, "presentFrame", Qt::QueuedConnection);
}

void WebcamWindow::presentFrame() {
    m_frameMutex.lock();
    m_viewport->setPixmap(QPixmap::fromImage(m_frame));
    m_frameMutex.unlock();
    m_viewport->repaint();
}

void WebcamWindow::changeResolution(int resolutionNum) {
    bool wasCapturing = m_isCapturing;
    stopCapture();
    m_videoCapture->changeActiveDeviceResolution(resolutionNum);
    if (wasCapturing) {
        startCapture();
    }
}

void WebcamWindow::changeDevice(int deviceNum) {
    bool wasCapturing = m_isCapturing;
    stopCapture();
    m_videoCapture->changeActiveDevice(deviceNum);
    if (wasCapturing) {
        startCapture();
    }
}

void WebcamWindow::startCapture() {
    if (m_videoCapture->startCapture()) {
        m_isCapturing = true;
    }
}

void WebcamWindow::stopCapture() {
    if (m_videoCapture->stopCapture()) {
        m_isCapturing = false;
    }
}

