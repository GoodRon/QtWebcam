#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include <windows.h>

class CaptureWindow
{
public:
	CaptureWindow();
	~CaptureWindow();

	void Show();
	void DrawCapture(int x, int y, int width, int height, int bpp, unsigned char* data);

protected:
	static CaptureWindow* instance;
	void MessageLoop();

private:
	HWND	window;
	HDC		backbuffer;
	HBITMAP	backbitmap;

	static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

#endif // CAPTURE_WINDOW_H
