/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>
#include <QApplication>

#include "WebcamWindow.h"
#include "VideoDevice.h"
#include "VideoCapture.h"

static WebcamWindow* mainWindow = nullptr;

void callback(unsigned char* data, int len, VideoDevice* device) {
    if (mainWindow) {
        mainWindow->processFrame(data, len, device);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WebcamWindow window;
    mainWindow = &window;
    window.show();

    VideoCapture vc;
    auto devices = vc.getDevices();

    for (auto& device: devices) {
        std::cout << "Device: " << device->getFriendlyName().c_str() << std::endl;
        device->setCallback(callback);
    }

    (*devices.begin())->setCallback(callback);

    (*devices.begin())->start();

    return a.exec();
}
