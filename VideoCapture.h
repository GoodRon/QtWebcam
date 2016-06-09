/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <vector>
#include <memory>

#include "VideoCaptureCallback.h"

struct IFilterGraph2;
struct ICaptureGraphBuilder2;
struct IMediaControl;
class VideoDevice;

/**
 * @brief Class for DirectShow control
 */
class VideoCapture {
public:
    /**
     * @brief Constructor
     *
     * @param callback function for video frame processing
     */
    VideoCapture(VideoCaptureCallback callback);

    /**
     * @brief Destructor
     */
    ~VideoCapture();

    /**
     * @brief Get list of accessible video devices
     *
     * @return std::vector<std::wstring>
     */
    std::vector<std::wstring> getDevicesNames() const;

    /**
     * @brief Get the number of active video device
     *
     * @return unsigned
     */
    unsigned getActiveDeviceNum() const;

    /**
     * @brief Get list of accessible resolutions for the active device
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> getActiveDeviceResolutions() const;

    /**
     * @brief Change current active device
     *
     * @param deviceNum device id in the list
     * @return bool
     */
    bool changeActiveDevice(unsigned deviceNum);

    /**
     * @brief Change current active resolution
     *
     * @param resolutionNum resolution id the list
     * @return bool
     */
    bool changeActiveDeviceResolution(unsigned resolutionNum);

    /**
     * @brief Start video capture
     *
     * @return bool
     */
    bool startCapture();

    /**
     * @brief Stop video capture
     *
     * @return bool
     */
    bool stopCapture();

private:
    /**
     * @brief Initialize DirectShow draphs
     *
     * @return bool
     */
    bool initializeGraph();

    /**
     * @brief Initialize video devices
     *
     * @return bool
     */
    bool initializeVideo();

    /**
     * @brief Disconnect device filters
     *
     * @param device
     * @return bool
     */
    void disconnectFilters(VideoDevice* device);

    /**
     * @brief Update device capabilities
     *
     * @param device
     * @return bool
     */
    bool updateDeviceCapabilities(VideoDevice* device);

    /**
     * @brief Run media control interface
     *
     * @return bool
     */
    bool runControl();

    /**
     * @brief Stop media control interface
     *
     * @return bool
     */
    bool stopControl();

private:
    /**
     * @brief IFilterGraph2 interface
     */
    IFilterGraph2* m_graph;

    /**
     * @brief ICaptureGraphBuilder2 interface
     */
    ICaptureGraphBuilder2* m_capture;

    /**
     * @brief IMediaControl interface
     */
    IMediaControl* m_control;

    /**
     * @brief Readiness for video capture
     */
    bool m_readyForCapture;

    /**
     * @brief Active device number in list
     */
    unsigned m_activeDeviceNum;

    /**
     * @brief List of available devices
     */
    std::vector<std::shared_ptr<VideoDevice>> m_devices;
};

#endif // VIDEO_CAPTURE_H
