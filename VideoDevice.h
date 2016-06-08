/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include <string>
#include <vector>

#include "SampleGrabber.h"

struct IBaseFilter;
struct IFilterGraph2;
class VideoCapture;
class VideoDevice;

typedef void (*VideoCaptureCallback)(unsigned char* data, int len, VideoDevice* dev);

class VideoDevice {
public:
    struct Properties {
        Properties() : width(0), height(0), pixelFormat(),
            isFlippedHorizontal(false), isFlippedVertical(false) {}
        LONG width;
        LONG height;
        GUID pixelFormat;
        bool isFlippedHorizontal;
        bool isFlippedVertical;
    };

    VideoDevice();
    ~VideoDevice();

    int getId() const;
    std::wstring getFriendlyName() const;
    std::vector<VideoDevice::Properties> getPropertiesList() const;
    VideoDevice::Properties getCurrentProperties() const;
    void setCallback(VideoCaptureCallback callback);
    bool start();
    bool stop();

private:
    int m_id;
    std::wstring m_friendlyName;
    std::wstring m_filterName;
    std::vector<VideoDevice::Properties> m_propertiesList;
    VideoDevice::Properties m_currentProperties;

    IBaseFilter* m_sourceFilter;
    IBaseFilter* m_sampleGrabberFilter;
    IBaseFilter* m_nullRenderer;

    ISampleGrabber* m_sampleGrabber;
    IFilterGraph2* m_graph;

    class CallbackHandler : public ISampleGrabberCB {
    public:
            CallbackHandler(VideoDevice* device);
            ~CallbackHandler();

            void SetCallback(VideoCaptureCallback callback);

            virtual HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample* sample);
            virtual HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE* buffer, long len);
            virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
            virtual ULONG STDMETHODCALLTYPE AddRef();
            virtual ULONG STDMETHODCALLTYPE Release();

    private:
            VideoCaptureCallback m_callback;
            VideoDevice* m_device;

    }* m_callbackHandler;

friend class VideoCapture;
};

#endif // VIDEO_DEVICE_H
