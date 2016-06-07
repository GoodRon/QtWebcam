#include "mainwindow.h"
#include <QApplication>

/*
#include <Dshow.h>

#include "samplegrabber.h"


IGraphBuilder *graph;
ICaptureGraphBuilder2*	capture;
IMediaControl*			control;

void initializeGraph() {

    HRESULT hr;

    CoInitialize(NULL);

    //create the FilterGraph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
                          IID_IFilterGraph2,(void**) &graph);
    if (hr < 0) throw hr;

    //create the CaptureGraphBuilder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,
                          CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**) &capture);
    if (hr < 0) throw hr;

    //get the controller for the graph
    hr = graph->QueryInterface(IID_IMediaControl, (void**) &control);
    if (hr < 0) throw hr;

    capture->SetFiltergraph(graph);
}

ICreateDevEnum*		dev_enum;
IEnumMoniker*		enum_moniker;
IMoniker*			moniker;
IPropertyBag*		pbag;
IBaseFilter*	sourcefilter;
WCHAR*			filtername;
char*			friendlyname;
IBaseFilter*	samplegrabberfilter;
IFilterGraph2*			fgraph;
ISampleGrabber* samplegrabber;
IBaseFilter*	nullrenderer;

#define MAX_DEVICE_NAME 80

void InitializeVideo()
{
    friendlyname = (char*) calloc(1, MAX_DEVICE_NAME * sizeof(char));
    filtername =  (WCHAR*) calloc(1, MAX_DEVICE_NAME * sizeof(WCHAR));

    HRESULT hr;
    VARIANT name;
    WCHAR lfiltername[MAX_DEVICE_NAME + 2];

    LONGLONG start=0, stop=MAXLONGLONG;

    //create an enumerator for video input devices
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**) &dev_enum);
    if (hr < 0) throw hr;

    hr = dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&enum_moniker,NULL);
    if (hr < 0) throw hr;
    if (hr == S_FALSE) return; //no devices found

    //get devices (max 8)
    int num_devices = 0;
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
//                VideoDevice* dev = devices+num_devices++;
                BSTR ptr = name.bstrVal;

                for (int c = 0; *ptr; c++, ptr++)
                {
                    //bit hacky, but i don't like to include ATL
                    filtername[c] = *ptr;
                    friendlyname[c] = *ptr & 0xFF;
                }

                //add a filter for the device
                hr = fgraph->AddSourceFilterForMoniker(moniker, 0, filtername, &sourcefilter);
                if (hr != S_OK) throw hr;

                //create a samplegrabber filter for the device
                hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&samplegrabberfilter);
                if (hr < 0) throw hr;

                //set mediatype on the samplegrabber
                hr = samplegrabberfilter->QueryInterface(IID_ISampleGrabber, (void**)&samplegrabber);
                if (hr != S_OK) throw hr;

                wcscpy(lfiltername, L"SG ");
                wcscpy(lfiltername+3, filtername);
                fgraph->AddFilter(samplegrabberfilter, lfiltername);

                //set the media type
                AM_MEDIA_TYPE mt;
                memset(&mt, 0, sizeof(AM_MEDIA_TYPE));

                mt.majortype	= MEDIATYPE_Video;
                mt.subtype		= MEDIASUBTYPE_RGB24;
                // setting the above to 32 bits fails consecutive Select for some reason
                // and only sends one single callback (flush from previous one ???)
                // must be deeper problem. 24 bpp seems to work fine for now.

                hr = samplegrabber->SetMediaType(&mt);
                if (hr != S_OK) throw hr;

                //add the callback to the samplegrabber
//                hr = samplegrabber->SetCallback(callbackhandler,0);
//                if (hr != S_OK) throw hr;

                //set the null renderer
                hr = CoCreateInstance(CLSID_NullRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**) &nullrenderer);
                if (hr < 0) throw hr;

                wcscpy(lfiltername, L"NR ");
                wcscpy(lfiltername+3, filtername);
                graph->AddFilter(nullrenderer, lfiltername);

                //set the render path
                #ifdef SHOW_DEBUG_RENDERER
                hr = capture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, sourcefilter, samplegrabberfilter, NULL);
                #else
                hr = capture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, sourcefilter, samplegrabberfilter, nullrenderer);
                #endif
                if (hr < 0) throw hr;

                //if the stream is started, start capturing immediatly
                hr = capture->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, sourcefilter, &start, &stop, 1,2);
                if (hr < 0) throw hr;

                //reference the graph
                graph = graph;

                int id = num_devices;
            }
            VariantClear(&name);
            pbag->Release();
        }
        moniker->Release();

        // for 1
        break;
    }
}
*/
/*
int main(int argc, char *argv[]) {
//    initializeGraph();
//    InitializeVideo();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
*/

#include <cstdio>
#include "videocapture.h"
#include "CaptureWindow.h"

CaptureWindow* cw;

void callback(unsigned char* data, int len, int bpp, VideoDevice* dev)
{
//    int id = dev->GetId()-1;
//    int x = 330 * (id%2) + 10;
//    int y = 250 * (id/2) + 10;

    cw->DrawCapture(0,0,1280,720,bpp,data);
}

int main()
{
     cw = new CaptureWindow();

    VideoCapture* vc		= new VideoCapture();
    VideoDevice* devices	= vc->GetDevices();
    int num_devices			= vc->NumDevices();

    for (int i=0; i<num_devices; i++)
    {
        printf("%s\n", devices[i].GetFriendlyName());
        devices[i].SetCallback(callback);
        devices[i].Start();
    }
    cw->Show();

    return 0;
}

