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

std::string getFormatName(GUID uid) {
        if (uid == MEDIASUBTYPE_ARGB32) {
                return "MEDIASUBTYPE_ARGB32";
        }
        if (uid == MEDIASUBTYPE_RGB32) {
                return "MEDIASUBTYPE_RGB32";
        }
        if (uid == MEDIASUBTYPE_RGB24) {
                return "MEDIASUBTYPE_RGB24";
        }
        if (uid == MEDIASUBTYPE_RGB555) {
                return "MEDIASUBTYPE_RGB555";
        }
    return "unknown";
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WebcamWindow window;
    mainWindow = &window;
    window.show();

    VideoCapture vc;
    vc.runControl();
    auto devices = vc.getDevices();

    for (auto& device: devices) {
        std::wcout << "Device: " << device->getFriendlyName().c_str() << std::endl;
        device->setCallback(callback);
    }

    (*devices.begin())->setCallback(callback);

    (*devices.begin())->start();

    // TEST
    vc.stopControl();
    auto list = (*devices.begin())->getPropertiesList();
    auto properties = *list.rbegin();
    std::cout << "New format " << getFormatName(properties.pixelFormat) << std::endl;
    std::cout << "Set w: " << properties.width << " h: " << properties.height << std::endl;
    bool result = (*devices.begin())->setCurrentProperties(properties);
    if (!result) {
        std::cout << "Can't change properties" << std::endl;
    }
    vc.runControl();

    return a.exec();
}
