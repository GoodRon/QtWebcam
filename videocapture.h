/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <vector>
#include <memory>

struct IFilterGraph2;
struct ICaptureGraphBuilder2;
struct IMediaControl;
class VideoDevice;

class VideoCapture {
public:
    VideoCapture();
    ~VideoCapture();

    std::vector<std::shared_ptr<VideoDevice>> getDevices() const;

private:
    bool initializeGraph();
    bool initializeVideo();
    bool updateDeviceCapabilities(VideoDevice* device);

private:
    IFilterGraph2* m_graph;
    ICaptureGraphBuilder2* m_capture;
    IMediaControl* m_control;

    bool m_playing;

    std::vector<std::shared_ptr<VideoDevice>> m_devices;
};

#endif // VIDEO_CAPTURE_H
