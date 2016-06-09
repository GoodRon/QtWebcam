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

/**
 * @brief Main window class
 */
class WebcamWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param parent
     */
    WebcamWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~WebcamWindow();

    /**
     * @brief Process new video frame
     *
     * @param data
     * @param len
     * @param device
     */
    void processFrame(const unsigned char* data, int len, VideoDevice* device);

private Q_SLOTS:
    /**
     * @brief Render last frame
     */
    void presentFrame();

    /**
     * @brief Change video resolution
     *
     * @param int resolution number
     */
    void changeResolution(int resolutionNum);

    /**
     * @brief Change video device
     *
     * @param int device number
     */
    void changeDevice(int deviceNum);

    /**
     * @brief Flip frame
     */
    void flipFrame();

    /**
     * @brief Start capture
     */
    void startCapture();

    /**
     * @brief Stop capture
     */
    void stopCapture();

private:
    /**
     * @brief Viewport
     */
    QLabel* m_viewport;

    /**
     * @brief Frame mutex
     */
    QMutex m_frameMutex;

    /**
     * @brief Current frame
     */
    QImage m_frame;

    /**
     * @brief Layout for control widgets
     */
    QVBoxLayout* m_controlLayout;

    /**
     * @brief Group of control widgets
     */
    QGroupBox* m_controlGroup;

    /**
     * @brief Main layout
     */
    QHBoxLayout* m_windowLayout;

    /**
     * @brief Main group
     */
    QGroupBox* m_windowGroup;

    /**
     * @brief Start button
     */
    QPushButton* m_startButton;

    /**
     * @brief Stop button
     */
    QPushButton* m_stopButton;

    /**
     * @brief Hint
     */
    QLabel* m_devicesLabel;

    /**
     * @brief List of available devices
     */
    QComboBox* m_devices;

    /**
     * @brief Hint
     */
    QLabel* m_resolutionsLabel;

    /**
     * @brief List of available resolutions
     */
    QComboBox* m_resolutions;

    /**
     * @brief Devices group
     */
    QGroupBox* m_devicesGroup;

    /**
     * @brief Devices layout
     */
    QVBoxLayout* m_devicesLayout;

    /**
     * @brief Vertical splitter
     */
    QSplitter* m_vsplitter;

    /**
     * @brief Flip button
     */
    QPushButton* m_flipButton;

    /**
     * @brief Video controller
     */
    VideoCapture* m_videoCapture;

    /**
     * @brief Capturing flag
     */
    bool m_isCapturing;

    /**
     * @brief Flipping flag
     */
    bool m_isFlipped;
};

#endif // WEBCAM_WINDOW_HXX
