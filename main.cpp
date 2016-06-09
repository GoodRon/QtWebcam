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
    QString pluginPath = app.applicationDirPath() + "/plugins";
    std::cout << "plugin path: " << pluginPath.toUtf8().constData() << std::endl;
    app.addLibraryPath(pluginPath);

    WebcamWindow window;
    mainWindow = &window;
    window.show();

    VideoCapture vc(callback);
	if (!vc.startCapture()) {
		std::cout << "Can't start capture" << std::endl;
	}
	
	vc.stopCapture();
    auto resolutions = vc.getActiveDeviceResolutions().size();
	if (!vc.changeActiveDeviceResolution(0)) {
		std::cout << "Can't change resolution" << std::endl;
	}
	vc.startCapture();

    return app.exec();
}
