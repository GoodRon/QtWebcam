/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <QApplication>

#include "WebcamWindow.hxx"

/*
#include "mainwindow.h"
#include <QBitmap>
#include <QSize>
#include <QByteArray>

//#include <cstdio>
//#include "videocapture.h"
//#include "CaptureWindow.h"

//CaptureWindow* cw;

int main(int argc, char *argv[]) {
//    VideoCapture* vc		= new VideoCapture();
//    VideoDevice* devices	= vc->GetDevices();
//    int num_devices			= vc->NumDevices();
//
//    for (int i=0; i<num_devices; i++)
//    {
//        printf("%s\n", devices[i].GetFriendlyName());
//        devices[i].SetCallback(callback);
//        devices[i].Start();
//    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.bindBitmap(bitmap);
    return a.exec();
}

//int main()
//{
//     cw = new CaptureWindow();

//    VideoCapture* vc		= new VideoCapture();
//    VideoDevice* devices	= vc->GetDevices();
//    int num_devices			= vc->NumDevices();

//    for (int i=0; i<num_devices; i++)
//    {
//        printf("%s\n", devices[i].GetFriendlyName());
//        devices[i].SetCallback(callback);
//        devices[i].Start();
//    }
//    cw->Show();

//    return 0;
//}

 static QBitmap* bitmap = nullptr;

void callback(unsigned char* data, int len, int bpp, VideoDevice* dev)
{
    if (!bitmap) {
        return;
    }

    QByteArray data(data, len);

    QSize size(1280,720);
    bitmap->swap(QBitmap::fromData(size, data, QImage::Format_RGB888));
}
*/

void callback() {

}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WebcamWindow window;
    window.show();
    return a.exec();
}
