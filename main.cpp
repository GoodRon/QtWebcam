#include "mainwindow.h"
#include <QApplication>
#include <QBitmap>
#include <QSize>

#include <cstdio>
#include "videocapture.h"
#include "CaptureWindow.h"

CaptureWindow* cw;

static QBitmap* bitmap = nullptr;

void callback(unsigned char* data, int len, int bpp, VideoDevice* dev)
{
//    int id = dev->GetId()-1;
//    int x = 330 * (id%2) + 10;
//    int y = 250 * (id/2) + 10;

//    cw->DrawCapture(0,0,1280,720,bpp,data);

    if (!bitmap) {
        return;
    }

    QSize size(1280,720);
    bitmap->swap(QBitmap::fromData(size, data, QImage::Format_RGB888));
}

int main(int argc, char *argv[]) {
    VideoCapture* vc		= new VideoCapture();
    VideoDevice* devices	= vc->GetDevices();
    int num_devices			= vc->NumDevices();

    for (int i=0; i<num_devices; i++)
    {
        printf("%s\n", devices[i].GetFriendlyName());
        devices[i].SetCallback(callback);
        devices[i].Start();
    }

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

