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
    QApplication app(argc, argv);
    QString pluginPath = app.applicationDirPath(); + "/plugins";
    std::cout << "plugin path: " << pluginPath.toUtf8().constData() << std::endl;
    app.addLibraryPath(pluginPath);

    WebcamWindow window;
    mainWindow = &window;
    window.show();

    VideoCapture vc(callback);
    vc.startCapture();
    auto resolutions = vc.getActiveDeviceResolutions().size();
    vc.changeActiveDeviceResolution(0);
    vc.changeActiveDeviceResolution(1);
    vc.changeActiveDeviceResolution(resolutions - 1);

    return app.exec();
}
