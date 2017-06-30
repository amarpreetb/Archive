#include <windows.h>
#include<stdint.h>


//TWO TYPES OF STATIC VARIABLES 
#define internal static
#define local_persist static
//GLOBAL VARIABLE
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;



typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void RenderWeirdGradient(int XOffSet, int YOffSet){
	
	int Width = BitmapWidth;
	int Height = BitmapHeight;
	
	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y) {
		uint8 *Pixel = (uint8 *)Row;
		for (int X = 0; X < BitmapWidth; ++X) {
			//PIXEL MEMORY RR GG BB XX

			*Pixel = (uint8)(X + XOffSet);
			++Pixel;

			*Pixel = (uint8)(Y + YOffSet);
			++Pixel;

			*Pixel = 0;
			++Pixel;

			*Pixel = 0;
			++Pixel;
		}
		Row += Pitch;
	}
}

internal void Win32ResizeDibSection(int Width, int Height) {
	
	if (BitmapMemory) {
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	//TODO BULLETPROOF THIS
	
	int BytesPerPixel = 4;
	int BitmapMemorySize = (Width*Height)*BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	// THIS MOVED TO THE FUNCTION
	
	/*
	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
		for (int Y = 0; Y < BitmapHeight; ++Y) {
			uint8 *Pixel = (uint8 *)Row;
			for (int X = 0; X < BitmapWidth; ++X) {
				//PIXEL MEMORY RR GG BB XX
				
				*Pixel = (uint8)X;
				++Pixel;

				*Pixel = (uint8)Y;
				++Pixel; 

				*Pixel = 0;
				++Pixel;

				*Pixel = 0;
				++Pixel;
			}
			Row += Pitch;
		}*/
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height) {
	
	int WindowWidth = ClientRect->right - ClientRect->left;
	int WindowHeight = ClientRect->bottom - ClientRect->top;

	StretchDIBits(DeviceContext,
		/*
		X, Y, Width, Height,
		X, Y, Width, Height,*/

		0,0, BitmapWidth, BitmapHeight,
		0, 0, WindowWidth, WindowHeight,
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY
	);
}

LRESULT CALLBACK Win32MainWindowCallback(
	HWND   Window,
	UINT   Massage,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Massage) {
	case WM_SIZE:
	{
		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		int Width = ClientRect.right - ClientRect.left;
		int Height = ClientRect.bottom - ClientRect.top;		
		Win32ResizeDibSection(Width, Height);
	}break;

	case WM_DESTROY:
	{
		Running = false;
		//OutputDebugStringA("WM_DESTROY\n");
	}break;

	case WM_CLOSE:
	{
		Running = false;
		PostQuitMessage(0);
		//OutputDebugStringA("WM_CLOSE\n");
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
		//local_persist DWORD Operation = WHITENESS;

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		
		Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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

int CALLBACK WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode)

{
	WNDCLASSA WindowClass = {};

		WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
		WindowClass.lpfnWndProc = Win32MainWindowCallback;
		WindowClass.hInstance = Instance;
		/////WindowClass.hIcon;
		WindowClass.lpszClassName = "HandmadeHeroWindowsClass";

		if (RegisterClassA(&WindowClass)) {
			HWND Window = CreateWindowExA(
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
			if (Window) {
				int XOffSet = 0;
				int YOffSet = 0;

				Running = true;
				while (Running) {
					MSG Message;
					if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
						if (Message.message == WM_QUIT) {
							Running = false;
							}
							TranslateMessage(&Message);
							DispatchMessage(&Message);
						}
					RenderWeirdGradient(XOffSet, YOffSet);
					HDC DeviceContext = GetDC(Window);
					RECT ClientRect;
					GetClientRect(Window, &ClientRect);
					int WindowWidth = ClientRect.right - ClientRect.left;
					int WindowHeight = ClientRect.bottom - ClientRect.top;
					Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
					ReleaseDC(Window, DeviceContext);

					++XOffSet;
					

				}
			}
			else {
				//TODO: Logging
			}
		}

		else {
			//TODO: Logging
		}
	return(0);
}