#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H


//#include <windows.h>
#include <dshow.h>
#include "samplegrabber.h"

#ifndef MAXLONGLONG
#define MAXLONGLONG 0x7FFFFFFFFFFFFFFF
#endif

#ifndef MAX_DEVICES
#define MAX_DEVICES 8
#endif

#ifndef MAX_DEVICE_NAME
#define MAX_DEVICE_NAME 80
#endif

#ifndef BITS_PER_PIXEL
#define BITS_PER_PIXEL 24
#endif

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} RGB;

class VideoDevice;

typedef void (*VideoCaptureCallback)(unsigned char* data, int len, int bitsperpixel, VideoDevice* dev);

class VideoDevice
{
public:
    VideoDevice();
    ~VideoDevice();

    int			 GetId();
	const char*	 GetFriendlyName();
	void		 SetCallback(VideoCaptureCallback cb);

	void			Start();
	void			Stop();

private:
    int				id;
	char*			friendlyname;
	WCHAR*			filtername;

	IBaseFilter*	sourcefilter;
	IBaseFilter*	samplegrabberfilter;
	IBaseFilter*	nullrenderer;

	ISampleGrabber* samplegrabber;

	IFilterGraph2*	graph;

	class CallbackHandler : public ISampleGrabberCB
	{
	public:
		CallbackHandler(VideoDevice* parent);
		~CallbackHandler();

		void SetCallback(VideoCaptureCallback cb);

		virtual HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample* sample);
		virtual HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE* buffer, long len);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, LPVOID *ppv );
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();

	private:
		VideoCaptureCallback	callback;
		VideoDevice*			parent;

	} *callbackhandler;

    friend class	VideoCapture;
};

class VideoCapture
{
public:
    VideoCapture();
    ~VideoCapture();

    VideoDevice* GetDevices();
    int  NumDevices();

protected:
    void InitializeGraph();
    void InitializeVideo();
	void printCapabilities();

private:
    IFilterGraph2*			graph;
    ICaptureGraphBuilder2*	capture;
    IMediaControl*			control;

    bool					playing;

    VideoDevice*			devices;
    VideoDevice*			current;

    int						num_devices;
};



#endif // VIDEOCAPTURE_H
