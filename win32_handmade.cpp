#include <windows.h>

LRESULT CALLBACK MainWindowCallback(
	HWND   Window,
	UINT   Massage,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Massage) {
	case WM_SIZE:
	{
		OutputDebugStringA("WM_SIZE\n");
	}break;

	case WM_DESTROY:
	{
		OutputDebugStringA("WM_DESTROY\n");
	}break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		OutputDebugStringA("WM_CLOSE\n");
	}break;
	 
	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	}break;
	
	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		static DWORD Operation = WHITENESS;
		PatBlt(DeviceContext, X, Y, Width, Height, WHITENESS);
		EndPaint(Window, &Paint);
	}break;
	default:
	{
		OutputDebugStringA("default\n");

		Result = DefWindowProcA(Window, Massage, WParam, LParam);
	}break;

	}
	return(Result);
}

int CALLBACK 
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode)

{
	WNDCLASSA WindowClass = {};

		WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
		WindowClass.lpfnWndProc = MainWindowCallback;
		WindowClass.hInstance = Instance;
		/////WindowClass.hIcon;
		WindowClass.lpszClassName = "HandmadeHeroWindowsClass";

		if (RegisterClassA(&WindowClass)) {
			HWND WindowHandle = 
				CreateWindowExA(
				0, 
				WindowClass.lpszClassName,
				"Window",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				Instance,
				0);
			if (WindowHandle) {
				for (;;) {
					MSG Message;
					BOOL MassgeResult = GetMessage(&Message, 0, 0,0);
					if (MassgeResult > 0) {
						TranslateMessage(&Message);
						DispatchMessage(&Message);
					}
					else {
						break;
					}
				}
			}
			else {
				//TODO(casey): Logging
			}
		}

		else {
			//TODO(casey): Logging
		}
	return(0);
}