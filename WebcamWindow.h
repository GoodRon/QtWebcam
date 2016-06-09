/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef WEBCAM_WINDOW_HXX
#define WEBCAM_WINDOW_HXX

#include <QMainWindow>
#include <QMutex>
#include <QImage>

class QLabel;
class VideoDevice;
class VideoCapture;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QComboBox;
class QGroupBox;
class QSplitter;

class WebcamWindow : public QMainWindow {
    Q_OBJECT

public:
    WebcamWindow(QWidget *parent = nullptr);
    virtual ~WebcamWindow();

    void processFrame(const unsigned char* data, int len, VideoDevice* device);

private Q_SLOTS:
    void presentFrame();
    void changeResolution(int resolutionNum);
    void changeDevice(int deviceNum);
    void flipFrame();
    void startCapture();
    void stopCapture();

private:
    QLabel* m_viewport;
    QMutex m_frameMutex;
    QImage m_frame;
    QVBoxLayout* m_controlLayout;
    QGroupBox* m_controlGroup;
    QHBoxLayout* m_windowLayout;
    QGroupBox* m_windowGroup;
    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QComboBox* m_devices;
    QComboBox* m_resolutions;
    QSplitter* m_vsplitter;
    QPushButton* m_flipButton;
    VideoCapture* m_videoCapture;
    bool m_isCapturing;
    bool m_isFlipped;
};


#endif // WEBCAM_WINDOW_HXX
