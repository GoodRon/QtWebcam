#include "videocapture.h"

//VIDEODEVICE
//public
VideoDevice::VideoDevice()
{
    friendlyname = (char*) calloc(1, MAX_DEVICE_NAME * sizeof(char));
    filtername =  (WCHAR*) calloc(1, MAX_DEVICE_NAME * sizeof(WCHAR));

    id					= -1;
    sourcefilter		= 0;
    samplegrabberfilter = 0;
    nullrenderer		= 0;
    callbackhandler		= new CallbackHandler(this);
}

VideoDevice::~VideoDevice()
{
    free(friendlyname);
    free(filtername);
}

int VideoDevice::GetId()
{
    return id;
}

const char* VideoDevice::GetFriendlyName()
{
    return friendlyname;
}

void VideoDevice::SetCallback(VideoCaptureCallback cb)
{
    callbackhandler->SetCallback(cb);
}

void VideoDevice::Start()
{
    HRESULT hr;

    hr = nullrenderer->Run(0);
    if (hr < 0) throw hr;

    hr = samplegrabberfilter->Run(0);
    if (hr < 0) throw hr;

    hr = sourcefilter->Run(0);
    if (hr < 0) throw hr;

}

void VideoDevice::Stop()
{
    HRESULT hr;

    hr = sourcefilter->Stop();
    if (hr < 0) throw hr;

    hr = samplegrabberfilter->Stop();
    if (hr < 0) throw hr;

    hr = nullrenderer->Stop();
    if (hr < 0) throw hr;

}

//VIDEODEVICE::CALLBACKHANDLER
//public
VideoDevice::CallbackHandler::CallbackHandler(VideoDevice* vd)
{
    callback = 0;
    parent = vd;
}

VideoDevice::CallbackHandler::~CallbackHandler()
{
}

void VideoDevice::CallbackHandler::SetCallback(VideoCaptureCallback cb)
{
    callback = cb;
}

HRESULT VideoDevice::CallbackHandler::SampleCB(double time, IMediaSample *sample)
{
    /*
    HRESULT hr;
    unsigned char* buffer;

    hr = sample->GetPointer((BYTE**)&buffer);
    if (hr != S_OK) return S_OK;

    if (callback) callback(buffer, sample->GetActualDataLength(), BITS_PER_PIXEL, parent);
    */
    return S_OK;
}

HRESULT VideoDevice::CallbackHandler::BufferCB(double time, BYTE *buffer, long len)
{
    if (callback) callback(buffer, len, BITS_PER_PIXEL, parent);
    return S_OK;
}

HRESULT VideoDevice::CallbackHandler::QueryInterface(const IID &iid, LPVOID *ppv)
{
    if( iid == IID_ISampleGrabberCB || iid == IID_IUnknown )
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*>( this );
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG VideoDevice::CallbackHandler::AddRef()
{
    return 1;
}

ULONG VideoDevice::CallbackHandler::Release()
{
    return 2;
}





//VIDEOCAPTURE
//public
VideoCapture::VideoCapture()
{
    CoInitialize(NULL);

    playing = false;
    current = 0;
    devices = new VideoDevice[MAX_DEVICES];

    InitializeGraph();
    InitializeVideo();
    printCapabilities();

    // we have to use this construct, because other
    // filters may have been added to the graph
    control->Run();
    for (int i=0; i<num_devices; i++)
    {
        devices[i].Stop();
    }
}

VideoCapture::~VideoCapture()
{
    delete[] devices;
}

VideoDevice* VideoCapture::GetDevices()
{
    return devices;
}

int VideoCapture::NumDevices()
{
    return num_devices;
}

//protected
void VideoCapture::InitializeGraph()
{
    HRESULT hr;

    //create the FilterGraph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IFilterGraph2,(void**) &graph);
    if (hr < 0) throw hr;

    //create the CaptureGraphBuilder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**) &capture);
    if (hr < 0) throw hr;

    //get the controller for the graph
    hr = graph->QueryInterface(IID_IMediaControl, (void**) &control);
    if (hr < 0) throw hr;

    capture->SetFiltergraph(graph);
}

void VideoCapture::InitializeVideo()
{
    HRESULT hr;
    VARIANT name;
    WCHAR filtername[MAX_DEVICE_NAME + 2];

    LONGLONG start=0, stop=MAXLONGLONG;

    ICreateDevEnum*		dev_enum;
    IEnumMoniker*		enum_moniker;
    IMoniker*			moniker;
    IPropertyBag*		pbag;

    //create an enumerator for video input devices
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**) &dev_enum);
    if (hr < 0) throw hr;

    hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&enum_moniker,NULL);
    if (hr < 0) throw hr;
    if (hr == S_FALSE) return; //no devices found

    //get devices (max 8)
    num_devices = 0;
    while (enum_moniker->Next(1, &moniker,0) == S_OK)
    {
        //get properties
        hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**) &pbag);
        if (hr >= 0)
        {
            VariantInit(&name);

            //get the description
            hr = pbag->Read(L"Description", &name, 0);
            if (hr < 0) hr = pbag->Read(L"FriendlyName", &name, 0);
            if (hr >= 0)
            {
                //Initialize the VideoDevice struct
                VideoDevice* dev = devices+num_devices++;
                BSTR ptr = name.bstrVal;

                for (int c = 0; *ptr; c++, ptr++)
                {
                    //bit hacky, but i don't like to include ATL
                    dev->filtername[c] = *ptr;
                    dev->friendlyname[c] = *ptr & 0xFF;
                }

                //add a filter for the device
                hr = graph->AddSourceFilterForMoniker(moniker, 0, dev->filtername, &dev->sourcefilter);
                if (hr != S_OK) throw hr;

                //create a samplegrabber filter for the device
                hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&dev->samplegrabberfilter);
                if (hr < 0) throw hr;

                //set mediatype on the samplegrabber
                hr = dev->samplegrabberfilter->QueryInterface(IID_ISampleGrabber, (void**)&dev->samplegrabber);
                if (hr != S_OK) throw hr;

                wcscpy(filtername, L"SG ");
                wcscpy(filtername+3, dev->filtername);
                graph->AddFilter(dev->samplegrabberfilter, filtername);

                //set the media type
                AM_MEDIA_TYPE mt;
                memset(&mt, 0, sizeof(AM_MEDIA_TYPE));

                mt.majortype	= MEDIATYPE_Video;
                mt.subtype		= MEDIASUBTYPE_RGB24;
                // setting the above to 32 bits fails consecutive Select for some reason
                // and only sends one single callback (flush from previous one ???)
                // must be deeper problem. 24 bpp seems to work fine for now.

                hr = dev->samplegrabber->SetMediaType(&mt);
                if (hr != S_OK) throw hr;

                //add the callback to the samplegrabber
                hr = dev->samplegrabber->SetCallback(dev->callbackhandler,1);
                if (hr != S_OK) throw hr;

                //set the null renderer
                hr = CoCreateInstance(CLSID_NullRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**) &dev->nullrenderer);
                if (hr < 0) throw hr;

                wcscpy(filtername, L"NR ");
                wcscpy(filtername+3, dev->filtername);
                graph->AddFilter(dev->nullrenderer, filtername);

                //set the render path
                #ifdef SHOW_DEBUG_RENDERER
                hr = capture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, dev->sourcefilter, dev->samplegrabberfilter, NULL);
                #else
                hr = capture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, dev->sourcefilter, dev->samplegrabberfilter, dev->nullrenderer);
                #endif
                if (hr < 0) throw hr;

                //if the stream is started, start capturing immediatly
                hr = capture->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, dev->sourcefilter, &start, &stop, 1,2);
                if (hr < 0) throw hr;

                //reference the graph
                dev->graph = graph;

                dev->id = num_devices;
            }
            VariantClear(&name);
            pbag->Release();
        }
        moniker->Release();
    }
}

#include <iostream>

bool checkPixelFormat(GUID uid) {
    if (uid == MEDIASUBTYPE_ARGB32)
        std::cout << "MEDIASUBTYPE_ARGB32" << std::endl;
    else if (uid == MEDIASUBTYPE_RGB32)
        std::cout << "MEDIASUBTYPE_RGB32" << std::endl;
    else if (uid == MEDIASUBTYPE_RGB24)
        std::cout << "MEDIASUBTYPE_RGB24" << std::endl;
    else if (uid == MEDIASUBTYPE_RGB565)
        std::cout << "MEDIASUBTYPE_RGB565" << std::endl;
    else if (uid == MEDIASUBTYPE_RGB555)
        std::cout << "MEDIASUBTYPE_RGB555" << std::endl;
    else if (uid == MEDIASUBTYPE_AYUV)
        std::cout << "MEDIASUBTYPE_AYUV" << std::endl;
    else if (uid == MEDIASUBTYPE_YV12)
        std::cout << "MEDIASUBTYPE_YV12" << std::endl;
    else if (uid == MEDIASUBTYPE_UYVY)
        std::cout << "MEDIASUBTYPE_UYVY" << std::endl;
    else if (uid == MEDIASUBTYPE_YUYV || uid == MEDIASUBTYPE_YUY2)
        std::cout << "MEDIASUBTYPE_YUYV" << std::endl;
    else if (uid == MEDIASUBTYPE_NV12)
        std::cout << "MEDIASUBTYPE_NV12" << std::endl;
    else if (uid == MEDIASUBTYPE_IMC1)
        std::cout << "MEDIASUBTYPE_IMC1" << std::endl;
    else if (uid == MEDIASUBTYPE_IMC2)
        std::cout << "MEDIASUBTYPE_IMC2" << std::endl;
    else if (uid == MEDIASUBTYPE_IMC3)
        std::cout << "MEDIASUBTYPE_IMC3" << std::endl;
    else if (uid == MEDIASUBTYPE_IMC4)
        std::cout << "MEDIASUBTYPE_IMC4" << std::endl;
    else {
        return false;
    }
    return true;
}

void VideoCapture::printCapabilities() {
    HRESULT hr;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig* pConfig = 0;

    VideoDevice* dev = devices;

    hr = capture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                dev->sourcefilter, IID_IAMStreamConfig, (void**)&pConfig);
    if (FAILED(hr)) {
        std::cerr << "Can't find IID_IAMStreamConfig" << std::endl;
        throw hr;
    }

    int iCount;
    int iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr)) {
        std::cerr << "Can't GetNumberOfCapabilities" << std::endl;
        throw hr;
    }

    for (int iIndex = 0; iIndex < iCount; ++iIndex) {
        hr = pConfig->GetStreamCaps(iIndex, &pmt, reinterpret_cast<BYTE*>(&scc));
        if (FAILED(hr)) {
            std::cerr << "Can't GetStreamCaps" << std::endl;
            throw hr;
        }

        if (!checkPixelFormat(pmt->subtype)) {
            continue;
        }

        if (pmt->majortype == MEDIATYPE_Video
            && pmt->formattype == FORMAT_VideoInfo) {
            pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
            std::cout << "width " << pvi->bmiHeader.biWidth
                      << " height " << pvi->bmiHeader.biHeight << std::endl;
        }

        if (pmt->majortype == MEDIATYPE_Video && pmt->subtype == MEDIASUBTYPE_RGB24 &&
            pmt->formattype == FORMAT_VideoInfo && pvi->bmiHeader.biHeight == 720) {
            hr = pConfig->SetFormat(pmt);
            if (FAILED(hr)) {
                std::cerr << "Can't GetStreamCaps" << std::endl;
                throw hr;
            }
        }
    }

    pConfig->Release();
}


