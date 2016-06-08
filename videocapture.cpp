/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <dshow.h>

#include "VideoDevice.h"
#include "VideoCapture.h"
#include "SampleGrabber.h"

#include <iostream>

using namespace std;

HRESULT getPin(IBaseFilter* pFilter, PIN_DIRECTION PinDir, IPin** ppPin) {
    *ppPin = nullptr;
    IEnumPins *pEnum = nullptr;
    IPin *pPin = nullptr;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) {
        return hr;
    }

    pEnum->Reset();
    while (pEnum->Next(1, &pPin, NULL) == S_OK) {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir) {
            pEnum->Release();
            *ppPin = pPin;
            return S_OK;
        }
        pPin->Release();
    }
    pEnum->Release();
    return E_FAIL;
}

VideoCapture::VideoCapture():
    m_graph(nullptr),
    m_capture(nullptr),
    m_control(nullptr),
    m_playing(false),
    m_devices() {
    CoInitialize(NULL);

	m_playing = false;

    initializeGraph();
    initializeVideo();

    // we have to use this construct, because other
    // filters may have been added to the graph
    m_control->Run();
    for (auto& device: m_devices) {
        device->stop();
    }
}

VideoCapture::~VideoCapture() {
}

std::vector<std::shared_ptr<VideoDevice>> VideoCapture::getDevices() const {
    return m_devices;
}

bool VideoCapture::initializeGraph() {
    HRESULT hr = S_FALSE;

    // create the FilterGraph
    hr = CoCreateInstance(CLSID_FilterGraph, nullptr,
                          CLSCTX_INPROC_SERVER, IID_IFilterGraph2,
                          reinterpret_cast<void**>(&m_graph));
    if (hr < 0 || !m_graph) {
        return false;
    }

    // create the CaptureGraphBuilder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr,
                          CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
                          reinterpret_cast<void**>(&m_capture));
    if (hr < 0 || !m_capture) {
        return false;
    }

    // get the controller for the graph
    hr = m_graph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&m_control));
    if (hr < 0 || !m_control) {
        return false;
    }
    m_capture->SetFiltergraph(m_graph);
    return true;
}

bool VideoCapture::initializeVideo() {
    HRESULT hr = S_FALSE;
    VARIANT name;
    wstring filterName;

    ICreateDevEnum* devEnum = nullptr;
    IEnumMoniker* enumMoniker = nullptr;
    IMoniker* moniker = nullptr;
    IPropertyBag* pbag = nullptr;

    // create an enumerator for video input devices
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr,
                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          reinterpret_cast<void**>(&devEnum));
    if (hr < 0 || !devEnum) {
        return false;
    }

    hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                        &enumMoniker, NULL);
    if (hr < 0 || !devEnum) {
        return false;
    }

    int devNum = 1;
    while (enumMoniker->Next(1, &moniker, 0) == S_OK) {
        hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag,
                                    reinterpret_cast<void**>(&pbag));
        if (hr >= 0) {
            VariantInit(&name);

            hr = pbag->Read(L"Description", &name, 0);
            if (hr < 0) {
                hr = pbag->Read(L"FriendlyName", &name, 0);
                if (hr < 0) {
                    moniker->Release();
                    continue;
                }
            }

            shared_ptr<VideoDevice> device(new VideoDevice);
            device->m_id = devNum;
            // TODO check
            std::wstring wname(name.bstrVal, SysStringLen(name.bstrVal));
            device->m_friendlyName = device->m_filterName = wname;

            // add a filter for the device
            hr = m_graph->AddSourceFilterForMoniker(moniker, nullptr, device->m_filterName.c_str(),
                                                    &device->m_sourceFilter);
            if (hr != S_OK) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // create a samplegrabber filter for the device
            hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IBaseFilter,
                                  reinterpret_cast<void**>(&device->m_sampleGrabberFilter));
            if (hr < 0) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // set mediatype on the samplegrabber
            hr = device->m_sampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                                               reinterpret_cast<void**>(&device->m_sampleGrabber));
            if (hr != S_OK) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // set device capabilities
            updateDeviceCapabilities(device.get());

            filterName = L"SG " + device->m_filterName;
            m_graph->AddFilter(device->m_sampleGrabberFilter, filterName.c_str());

            // set the media type
            AM_MEDIA_TYPE mt;
            memset(&mt, 0, sizeof(AM_MEDIA_TYPE));

            mt.majortype = MEDIATYPE_Video;
            mt.subtype = device->getCurrentProperties().pixelFormat;

            hr = device->m_sampleGrabber->SetMediaType(&mt);
            if (hr != S_OK) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // add the callback to the samplegrabber
            hr = device->m_sampleGrabber->SetCallback(device->m_callbackHandler,1);
            if (hr != S_OK) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // set the null renderer
            hr = CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IBaseFilter, reinterpret_cast<void**>(&device->m_nullRenderer));
            if (hr < 0) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            filterName = L"NR " + device->m_filterName;
            m_graph->AddFilter(device->m_nullRenderer, filterName.c_str());

            hr = m_capture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                         device->m_sourceFilter,
                                         device->m_sampleGrabberFilter,
                                         device->m_nullRenderer);
            if (hr < 0) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // if the stream is started, start capturing immediatly
            LONGLONG start=0, stop=MAXLONGLONG;
            hr = m_capture->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                          device->m_sourceFilter, &start, &stop, 1, 2);
            if (hr < 0) {
                pbag->Release();
                moniker->Release();
                continue;
            }

            // reference the graph
            device->m_graph = m_graph;
            m_devices.push_back(device);

            VariantClear(&name);
            pbag->Release();
        }
        moniker->Release();
    }
    enumMoniker->Release();
    devEnum->Release();
    return true;
}

bool checkPixelFormat(GUID uid) {
    if (uid == MEDIASUBTYPE_ARGB32 ||
            uid == MEDIASUBTYPE_RGB32 ||
            uid == MEDIASUBTYPE_RGB24 ||
            uid == MEDIASUBTYPE_RGB555) {
        return true;
    }
    return false;
}

bool VideoCapture::updateDeviceCapabilities(VideoDevice* device) {
    if (!device) {
        return false;
    }

    HRESULT hr = S_FALSE;
    AM_MEDIA_TYPE* pmt = nullptr;
    VIDEOINFOHEADER* pvi = nullptr;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig* pConfig = nullptr;

    hr = m_capture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                  device->m_sourceFilter, IID_IAMStreamConfig,
                                  reinterpret_cast<void**>(&pConfig));
    if (hr < 0) {
        std::cerr << "Can't find IID_IAMStreamConfig" << std::endl;
        return false;
    }

    int iCount = 0;
    int iSize = 0;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (hr < 0) {
        std::cerr << "Can't GetNumberOfCapabilities" << std::endl;
        pConfig->Release();
        return false;
    }

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (hr < 0) {
            std::cerr << "Can't GetStreamCaps" << std::endl;
            continue;
        }

        if (!checkPixelFormat(pmt->subtype)) {
            continue;
        }

        VideoDevice::Properties properties;
        if (pmt->majortype == MEDIATYPE_Video &&
                pmt->formattype == FORMAT_VideoInfo) {
            pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);

            properties.width = pvi->bmiHeader.biWidth;
            properties.height = pvi->bmiHeader.biHeight;
            properties.pixelFormat = pmt->subtype;

            std::cout << "width " << pvi->bmiHeader.biWidth
                      << " height " << pvi->bmiHeader.biHeight << std::endl;
        }

        IAMVideoControl* pVideoControl = nullptr;
        hr = m_capture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      device->m_sourceFilter, IID_IAMVideoControl,
                                      reinterpret_cast<void**>(&pVideoControl));
        if (hr < 0) {
            std::cerr << "Can't FindInterface(IID_IAMVideoControl)" << std::endl;
            continue;
        }

        IPin* pPin = nullptr;
        hr = getPin(device->m_sourceFilter, PINDIR_OUTPUT, &pPin);
        if (hr < 0) {
            std::cerr << "Can't getPin" << std::endl;
            continue;
        }

        long supportedModes;
        hr = pVideoControl->GetCaps(pPin, &supportedModes);
        if (hr < 0) {
            std::cerr << "Can't GetCaps" << std::endl;
            pPin->Release();
            pVideoControl->Release();
            continue;
        }

        long mode;
        hr = pVideoControl->GetMode(pPin, &mode);
        if (hr < 0) {
            std::cerr << "Can't GetMode" << std::endl;
            pPin->Release();
            pVideoControl->Release();
            continue;
        }

        properties.isFlippedHorizontal = mode & VideoControlFlag_FlipHorizontal;
        properties.isFlippedVertical = mode & VideoControlFlag_FlipVertical;
        device->m_propertiesList.push_back(properties);

        pPin->Release();
        pVideoControl->Release();
    }

    if (!device->m_propertiesList.empty()) {
        device->m_currentProperties = *device->m_propertiesList.begin();
    }

    pConfig->Release();
    return true;
}
