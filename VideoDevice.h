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

/**
 * @brief Video device
 */
class VideoDevice {
public:
    /**
     * @brief Video device properties
     */
    struct Properties {
        /**
         * @brief Constructor
         */
        Properties() : mediaType(), width(0), height(0), pixelFormat(),
            isFlippedHorizontal(false), isFlippedVertical(false) {}

        /**
         * @brief Media type structure
         */
        AM_MEDIA_TYPE mediaType;

        /**
         * @brief Frame width
         */
        LONG width;

        /**
         * @brief Frame height
         */
        LONG height;

        /**
         * @brief Frame pixel format
         */
        GUID pixelFormat;

        /**
         * @brief Necessity of horizontal flipping
         */
        bool isFlippedHorizontal;

        /**
         * @brief Necessity of vertical flipping
         */
        bool isFlippedVertical;
    };

    /**
     * @brief Constructor
     */
    VideoDevice();

    /**
     * @brief Destructor
     */
    ~VideoDevice();

    /**
     * @brief Get device id
     *
     * @return int
     */
    int getId() const;

    /**
     * @brief Get device name
     *
     * @return std::wstring
     */
    std::wstring getFriendlyName() const;

    /**
     * @brief Get device name
     *
     * @return std::wstring
     */
    std::vector<VideoDevice::Properties> getPropertiesList() const;

    /**
     * @brief Get current frame properties
     *
     * @return Properties
     */
    VideoDevice::Properties getCurrentProperties() const;

    /**
     * @brief Set current frame properties
     *
     * @param properties
     * @return bool
     */
    bool setCurrentProperties(const VideoDevice::Properties& properties);

    /**
     * @brief Set function that process video frames
     *
     * @param callback
     * @return bool
     */
    void setCallback(VideoCaptureCallback& callback);

    /**
     * @brief Start capturing
     *
     * @return bool
     */
    bool start();

    /**
     * @brief Stop capturing
     *
     * @return bool
     */
    bool stop();

    /**
     * @brief Check if active
     *
     * @return bool
     */
    bool isActive() const;

private:
    /**
     * @brief Device id
     */
    int m_id;

    /**
     * @brief Device name
     */
    std::wstring m_friendlyName;

    /**
     * @brief Filter name
     */
    std::wstring m_filterName;

    /**
     * @brief List of properties
     */
    std::vector<VideoDevice::Properties> m_propertiesList;

    /**
     * @brief Current properties
     */
    VideoDevice::Properties m_currentProperties;

    /**
     * @brief Source filter
     */
    IBaseFilter* m_sourceFilter;

    /**
     * @brief Grab filter
     */
    IBaseFilter* m_sampleGrabberFilter;

    /**
     * @brief Null renderer
     */
    IBaseFilter* m_nullRenderer;

    /**
     * @brief Sample grabber
     */
    ISampleGrabber* m_sampleGrabber;

    /**
     * @brief IFilterGraph2 interface
     */
    IFilterGraph2* m_graph;

    /**
     * @brief IAMStreamConfig interface
     */
    IAMStreamConfig* m_config;

    /**
     * @brief Alive flag
     */
    bool m_isActive;

    /**
     * @brief Callback handler
     */
    class CallbackHandler : public ISampleGrabberCB {
    public:
        /**
         * @brief Constructor
         *
         * @param device
         */
        CallbackHandler(VideoDevice* device);

        /**
         * @brief Destructor
         */
        virtual ~CallbackHandler();

        /**
         * @brief Set function that process video frames
         *
         * @param callback
         */
        void SetCallback(VideoCaptureCallback& callback);

        /**
         * @overload
         */
        virtual HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample* sample);

        /**
         * @overload
         */
        virtual HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE* buffer, long len);

        /**
         * @overload
         */
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);

        /**
         * @overload
         */
        virtual ULONG STDMETHODCALLTYPE AddRef();

        /**
         * @overload
         */
        virtual ULONG STDMETHODCALLTYPE Release();

    private:
        /**
         * @brief Callback function
         */
        VideoCaptureCallback m_callback;

        /**
         * @brief Parent device
         */
        VideoDevice* m_device;

    }* m_callbackHandler;

    friend class VideoCapture;
};

#endif // VIDEO_DEVICE_H
