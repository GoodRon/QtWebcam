/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <dshow.h>

#include "VideoDevice.h"

VideoDevice::VideoDevice():
    m_id(-1),
    m_friendlyName(),
    m_filterName(),
    m_propertiesList(),
    m_currentProperties(),
    m_sourceFilter(nullptr),
    m_sampleGrabberFilter(nullptr),
    m_nullRenderer(nullptr),
    m_sampleGrabber(nullptr),
    m_graph(nullptr),
    m_config(nullptr),
    m_isActive(false),
    m_callbackHandler(nullptr) {
    m_callbackHandler = new CallbackHandler(this);
}

VideoDevice::~VideoDevice() {
    delete m_callbackHandler;

    if (m_sourceFilter) {
        m_sourceFilter->Release();
        m_sourceFilter = nullptr;
    }

    if (m_sampleGrabberFilter) {
        m_sampleGrabberFilter->Release();
        m_sampleGrabberFilter = nullptr;
    }

    if (m_sampleGrabber) {
        m_sampleGrabber->Release();
        m_sampleGrabber = nullptr;
    }

    if (m_nullRenderer) {
        m_nullRenderer->Release();
        m_nullRenderer = nullptr;
    }

    if (m_config) {
        m_config->Release();
        m_config = nullptr;
    }
}

int VideoDevice::getId() const {
    return m_id;
}

std::wstring VideoDevice::getFriendlyName() const {
    return m_friendlyName;
}

std::vector<VideoDevice::Properties> VideoDevice::getPropertiesList() const {
    return m_propertiesList;
}

VideoDevice::Properties VideoDevice::getCurrentProperties() const {
    return m_currentProperties;
}

bool VideoDevice::setCurrentProperties(const VideoDevice::Properties& properties) {
    m_currentProperties = properties;

    AM_MEDIA_TYPE mt = properties.mediaType;
    HRESULT hr = m_config->SetFormat(&mt);
    if (hr < 0) {
        return false;
    }
    return true;
}

void VideoDevice::setCallback(VideoCaptureCallback callback) {
    m_callbackHandler->SetCallback(callback);
}

bool VideoDevice::start(){
    HRESULT hr = S_FALSE;
    if (m_nullRenderer) {
        hr = m_nullRenderer->Run(0);
        if (hr < 0) {
            return false;
        }
    }

    if (m_sampleGrabberFilter) {
        hr = m_sampleGrabberFilter->Run(0);
        if (hr < 0) {
            return false;
        }
    }

    if (m_sourceFilter) {
        hr = m_sourceFilter->Run(0);
        if (hr < 0) {
            return false;
        }
    }
    m_isActive = true;
    return true;
}

bool VideoDevice::stop() {
    m_isActive = false;
    HRESULT hr = S_FALSE;

    if (m_sourceFilter) {
        hr = m_sourceFilter->Stop();
        if (hr < 0) {
            return false;
        }
    }

    if (m_sampleGrabberFilter) {
        hr = m_sampleGrabberFilter->Stop();
        if (hr < 0) {
            return false;
        }
    }

    if (m_nullRenderer) {
        hr = m_nullRenderer->Stop();
        if (hr < 0) {
            return false;
        }
    }
    return true;
}

bool VideoDevice::isActive() const {
    return m_isActive;
}

VideoDevice::CallbackHandler::CallbackHandler(VideoDevice* device):
    m_callback(),
    m_device(device) {
}

VideoDevice::CallbackHandler::~CallbackHandler() {
}

void VideoDevice::CallbackHandler::SetCallback(VideoCaptureCallback callback) {
    m_callback = callback;
}

HRESULT VideoDevice::CallbackHandler::SampleCB(double, IMediaSample*) {
    return S_OK;
}

HRESULT VideoDevice::CallbackHandler::BufferCB(double time, BYTE *buffer, long len) {
    if (m_callback) {
        m_callback(buffer, len, m_device);
    }
    return S_OK;
}

HRESULT VideoDevice::CallbackHandler::QueryInterface(REFIID iid, LPVOID *ppv) {
    if(iid == IID_ISampleGrabberCB || iid == IID_IUnknown) {
        *ppv = (void *) static_cast<ISampleGrabberCB*>(this);
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG VideoDevice::CallbackHandler::AddRef() {
    return 1;
}

ULONG VideoDevice::CallbackHandler::Release() {
    return 2;
}
