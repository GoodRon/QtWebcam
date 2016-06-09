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

    WebcamWindow window;
    window.show();
    return app.exec();
}
