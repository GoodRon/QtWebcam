/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include <string>
#include <vector>

#include "SampleGrabber.h"
#include "VideoCaptureCallback.h"

struct IBaseFilter;
struct IFilterGraph2;
class VideoCapture;
class VideoDevice;

class VideoDevice {
public:
    struct Properties {
        Properties() : mediaType(), width(0), height(0), pixelFormat(),
            isFlippedHorizontal(false), isFlippedVertical(false) {}
        AM_MEDIA_TYPE mediaType;
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
    bool setCurrentProperties(const VideoDevice::Properties& properties);
    void setCallback(VideoCaptureCallback callback);
    bool start();
    bool stop();
    bool isActive() const;

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
    IAMStreamConfig* m_config;
    bool m_isActive;

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
