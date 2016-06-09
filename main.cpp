/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <iostream>
#include <QApplication>

#include "WebcamWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QString pluginPath = app.applicationDirPath() + "/plugins";
    app.addLibraryPath(pluginPath);

    WebcamWindow& window = WebcamWindow::getInstance();
    window.show();

//    VideoCapture vc(callback);
//	if (!vc.startCapture()) {
//		std::cout << "Can't start capture" << std::endl;
//	}
//
//	vc.stopCapture();
//    auto resolutions = vc.getActiveDeviceResolutions().size();
//	if (!vc.changeActiveDeviceResolution(0)) {
//		std::cout << "Can't change resolution" << std::endl;
//	}
//	vc.startCapture();

    return app.exec();
}
