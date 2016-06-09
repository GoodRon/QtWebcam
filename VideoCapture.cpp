/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <dshow.h>

#include "VideoDevice.h"
#include "VideoCapture.h"
#include "SampleGrabber.h"

#include <sstream>

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

std::string getFormatName(GUID uid) {
    if (uid == MEDIASUBTYPE_ARGB32) {
        return "ARGB32";
    }
    if (uid == MEDIASUBTYPE_RGB32) {
        return "RGB32";
    }
    if (uid == MEDIASUBTYPE_RGB24) {
        return "RGB24";
    }
    if (uid == MEDIASUBTYPE_RGB555) {
        return "RGB555";
    }
    return "unknown format";
}

VideoCapture::VideoCapture(VideoCaptureCallback callback):
    m_graph(nullptr),
    m_capture(nullptr),
    m_control(nullptr),
    m_readyForCapture(false),
    m_activeDeviceNum(0),
    m_devices() {
    CoInitialize(NULL);
    initializeGraph();
    initializeVideo();

    for (auto& device: m_devices) {
        device->setCallback(callback);
    }
}

VideoCapture::~VideoCapture() {
	for (auto& device : m_devices) {
		device->stop();
	}

    stopControl();
	
	/*
    if (m_graph) {
        m_graph->Release();
    }
	*/
}

std::vector<std::wstring> VideoCapture::getDevicesNames() const {
    vector<wstring> names;
    for (auto& device: m_devices) {
        names.push_back(device->getFriendlyName());
    }
    return names;
}

std::vector<std::string> VideoCapture::getActiveDeviceResolutions() const {
    vector<string> resolutions;
    if (m_activeDeviceNum >= m_devices.size()) {
        return resolutions;
    }

    auto propertiesList = m_devices[m_activeDeviceNum]->getPropertiesList();
    for (auto& properties: propertiesList) {
        stringstream stream;
        stream << properties.width << "x" << properties.height << " "
               << getFormatName(properties.pixelFormat);
        string resolution;
        stream >> resolution;
        resolutions.push_back(resolution);
    }
    return resolutions;
}

bool VideoCapture::changeActiveDevice(unsigned deviceNum) {
    if (!stopCapture()) {
        return false;
    }
    if (deviceNum >= m_devices.size()) {
        return false;
    }
    m_activeDeviceNum = deviceNum;
    return true;
}

bool VideoCapture::changeActiveDeviceResolution(unsigned resolutionNum) {
    if (m_activeDeviceNum >= m_devices.size()) {
        return false;
    }

	if (m_readyForCapture) {
		if (!stopControl()) {
			return false;
		}
	}

    auto propertiesList = m_devices[m_activeDeviceNum]->getPropertiesList();
    if (resolutionNum >= propertiesList.size()) {
        return false;
    }
    if (!m_devices[m_activeDeviceNum]->setCurrentProperties(propertiesList[resolutionNum])) {
        return false;
    }

	if (m_readyForCapture) {
		if (!runControl()) {
			return false;
		}
	}
    return true;
}

bool VideoCapture::startCapture() {
    if (!m_readyForCapture) {
        if (!runControl()) {
            return false;
        }
    }

	if (m_activeDeviceNum >= m_devices.size()) {
		return false;
	}
    m_devices[m_activeDeviceNum]->start();
    return true;
}

bool VideoCapture::stopCapture() {
    if (m_activeDeviceNum >= m_devices.size()) {
        return false;
    }
    m_devices[m_activeDeviceNum]->stop();
    return true;
}

bool VideoCapture::runControl() {
	HRESULT hr = m_control->Run();
	if (hr < 0) {
		return false;
	}
    m_readyForCapture = true;
	return true;
}

bool VideoCapture::stopControl() {
    for (auto& device: m_devices) {
        device->stop();
    }
    m_readyForCapture = false;
	HRESULT hr = m_control->Stop();
	if (hr < 0) {
		return false;
	}
	return true;
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
        return false;
    }

    if (device->m_config) {
        device->m_config->Release();
    }
    device->m_config = pConfig;

    int iCount = 0;
    int iSize = 0;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (hr < 0) {
        pConfig->Release();
        return false;
    }

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (hr < 0) {
            continue;
        }

        if (!checkPixelFormat(pmt->subtype)) {
            continue;
        }

        VideoDevice::Properties properties;
        if (pmt->majortype == MEDIATYPE_Video &&
                pmt->formattype == FORMAT_VideoInfo) {
            pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);

            properties.mediaType = *pmt;
            properties.width = pvi->bmiHeader.biWidth;
            properties.height = pvi->bmiHeader.biHeight;
            properties.pixelFormat = pmt->subtype;
        }

        IAMVideoControl* pVideoControl = nullptr;
        hr = m_capture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      device->m_sourceFilter, IID_IAMVideoControl,
                                      reinterpret_cast<void**>(&pVideoControl));
        if (hr < 0) {
            continue;
        }

        IPin* pPin = nullptr;
        hr = getPin(device->m_sourceFilter, PINDIR_OUTPUT, &pPin);
        if (hr < 0) {
            continue;
        }

        long supportedModes;
        hr = pVideoControl->GetCaps(pPin, &supportedModes);
        if (hr < 0) {
            pPin->Release();
            pVideoControl->Release();
            continue;
        }

        long mode;
        hr = pVideoControl->GetMode(pPin, &mode);
        if (hr < 0) {
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
