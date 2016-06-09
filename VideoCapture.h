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

class VideoCapture {
public:
    VideoCapture(VideoCaptureCallback callback);
    ~VideoCapture();

    std::vector<std::wstring> getDevicesNames() const;
    unsigned getActiveDeviceNum() const;
    std::vector<std::string> getActiveDeviceResolutions() const;
    bool changeActiveDevice(unsigned deviceNum);
    bool changeActiveDeviceResolution(unsigned resolutionNum);

    bool startCapture();
    bool stopCapture();

private:
    bool initializeGraph();
    bool initializeVideo();
    bool updateDeviceCapabilities(VideoDevice* device);
    bool runControl();
    bool stopControl();

private:
    IFilterGraph2* m_graph;
    ICaptureGraphBuilder2* m_capture;
    IMediaControl* m_control;

    bool m_readyForCapture;
    unsigned m_activeDeviceNum;
    std::vector<std::shared_ptr<VideoDevice>> m_devices;
};

#endif // VIDEO_CAPTURE_H
