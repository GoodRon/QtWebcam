#include "CaptureWindow.h"

struct WindowInstance
{
	HWND window;
	CaptureWindow* instance;
	struct WindowInstance *next;
};

WindowInstance *root = 0;

CaptureWindow* GetInstance(HWND hwnd)
{
	WindowInstance* ptr = root;
	while (ptr && ptr->window != hwnd) ptr = ptr->next;
	return ptr ? ptr->instance : 0;
}

CaptureWindow::CaptureWindow()
{
	HINSTANCE hinst = GetModuleHandle(NULL);

	//Register Window Class
	WNDCLASSEX windowclass;
	memset(&windowclass,0, sizeof(WNDCLASSEX));
	
	windowclass.cbSize			= sizeof(WNDCLASSEX);
	windowclass.style			= CS_HREDRAW | CS_VREDRAW;
	windowclass.lpfnWndProc		= MessageHandler;
	windowclass.hInstance		= hinst;
	windowclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	windowclass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    windowclass.lpszMenuName	= "Main Menu";
    windowclass.lpszClassName	= "CaptureWindow";
	windowclass.hIconSm			= (HICON) LoadImage(hinst, MAKEINTRESOURCE(5), IMAGE_ICON, GetSystemMetrics(SM_CYSMICON), GetSystemMetrics(SM_CXSMICON), LR_DEFAULTCOLOR);

	RegisterClassEx(&windowclass);

	//Create the Window
    window = CreateWindow(	"CaptureWindow", "Limitz Capture Window",
							WS_OVERLAPPEDWINDOW,
							0, 0, 680, 600,
							NULL, NULL, hinst, NULL);

	//Create a compatible backbuffer
	HDC current = GetDC(window);
	RECT clientrect;
	
	GetClientRect(window, &clientrect);
	backbuffer = CreateCompatibleDC(current);
	backbitmap = CreateCompatibleBitmap(current, clientrect.right, clientrect.bottom);
	SelectObject(backbuffer,(HGDIOBJ)backbitmap);
	ReleaseDC(window, current);
	
	//add hwnd to instance reference
	WindowInstance** ptr = &root;
	while (*ptr) ptr = &(*ptr)->next;
	*ptr = (WindowInstance*)calloc(1, sizeof(WindowInstance));
	(*ptr)->window = window;
	(*ptr)->instance = this;
}

void CaptureWindow::Show()
{
	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);
	MessageLoop();
}

void CaptureWindow::DrawCapture(int x, int y, int width, int height, int bpp, unsigned char* data)
{
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bpp;
	bmi.bmiHeader.biCompression = BI_RGB;

	SetDIBitsToDevice(backbuffer,x,y,width,height, 0, 0, 0, (UINT)height, data, &bmi, DIB_RGB_COLORS);

	RECT invalid = {x,y,x+width,y+height};
	InvalidateRect(window, &invalid, false);
}

void CaptureWindow::MessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CaptureWindow::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			RECT clientrect;

			GetClientRect(hwnd, &clientrect);
			CaptureWindow* cw = GetInstance(hwnd);
			
			HDC hdc = BeginPaint(hwnd, &ps);
			BitBlt(hdc, 0, 0, clientrect.right, clientrect.bottom, cw->backbuffer, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);

			return 0;
		}

		default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

