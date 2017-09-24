#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>
#include<math.h>

//TWO TYPES OF STATIC VARIABLES 
#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

//SIGNED INTEGERS
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

//UNSIGNED INTEGERS
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

/*
IEEE floating point
https://en.wikipedia.org/wiki/IEEE_754
*/

typedef  float real32;
typedef double real64;

//=====================================================================================================

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return (0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

//======================================================================================================

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub) 
{
	return (0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

//======================================================================================================

/*
Direct Sound from "dsound.h"
*/
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name( LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

struct win32OffScreenBuffer 
{

	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

struct win32WindowDimension
{
	int Width;
	int Height;

};

global_variable win32OffScreenBuffer GlobalBackBuffer;
global_variable bool GlobalRunning;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

internal void  Win32LoadXInput(void) 
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");

	if (!XInputLibrary) 
	{
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
		if (XInputLibrary) 
		{
			XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
			if (!XInputGetState) 
			{
				XInputGetState = XInputGetStateStub;
			}
			XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
			if (!XInputSetState) 
			{
				XInputSetState = XInputSetStateStub;
			}
		}
	}

}

internal void Win32InitDSound(HWND Window, int32 SamplesPerSecond, int32 BufferSize) 
{
	//Load the Library
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

	if (DSoundLibrary)
	{
		//Get a directsound Object
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) 
		{

			/********************************************************************************
			WAVEFORMATEX structure Info available on MSDN
			https://msdn.microsoft.com/en-us/library/windows/desktop/dd390970(v=vs.85).aspx
			*********************************************************************************/

			WAVEFORMATEX WaveFormat = {};

			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = SamplesPerSecond;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.cbSize = 0;
			if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) 
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;


				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) 
				{
					HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
					if (SUCCEEDED(Error)) 
					{
						OutputDebugStringA("Primary Buffer");
					}
					else 
					{
						//diagnostics
					}

				}
				else 
				{
					//diagnostics
				}
			}
			else 
			{
				//diagnostics
			}
			//

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
			LPDIRECTSOUNDBUFFER SecondaryBuffer;
			HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0);
			if (SUCCEEDED(Error)) 
			{
				OutputDebugStringA("Secondary buffer created successfuly\n");
			}

		}
		else 
		{
			//diagnostics
		}
	}
}
internal win32WindowDimension Win32GetWindowDimension(HWND Window)
{
	win32WindowDimension Result;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return(Result);
}

internal void RenderWeirdGradient(win32OffScreenBuffer *Buffer, int BlueOffSet, int GreenOffSet)
{	
	
	uint8 *Row = (uint8 *)Buffer->Memory;
	for (int Y = 0; Y < Buffer->Height; ++Y) 
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Buffer->Width; ++X) 
		{
			//PIXEL MEMORY RR GG BB XX

			uint8 Blue = (X + BlueOffSet);
			uint8 Green = (Y + GreenOffSet);


			*Pixel++ = ((Green << 8) | Blue);
			
		}
		Row += Buffer->Pitch;
	}
}

internal void Win32ResizeDibSection(win32OffScreenBuffer *Buffer, int Width, int Height) 
{
	
	if (Buffer->Memory) 
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
	Buffer->Width = Width;
	Buffer->Height = Height;
	int BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width*BytesPerPixel;
	
}

internal void Win32DisplayBufferInWindow(win32OffScreenBuffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight)
{

	StretchDIBits(DeviceContext,
		/*
		X, Y, Width, Height,
		X, Y, Width, Height,*/
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS, SRCCOPY
	);
}

internal LRESULT CALLBACK Win32MainWindowCallback(
	HWND   Window,
	UINT   Massage,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Massage) 
	{

	case WM_CLOSE:
	{
		GlobalRunning = false;
		//PostQuitMessage(0);
		//OutputDebugStringA("WM_CLOSE\n");
	}break;

	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	}break;

	case WM_SIZE:
	{
		win32WindowDimension Dimension = Win32GetWindowDimension(Window);		
		Win32ResizeDibSection(&GlobalBackBuffer, Dimension.Width, Dimension.Height);
	}break;

	case WM_DESTROY:
	{
		GlobalRunning = false;
		//OutputDebugStringA("WM_DESTROY\n");
	}break;
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		uint32 VKCode = WParam;
		bool WasDown = ((LParam & (1 << 30)) != 0);
		bool IsDown = ((LParam & (1 << 31)) == 0);
		if (WasDown != IsDown) 
		{
			if (VKCode == 'W') 
			{

			}
			else if (VKCode == 'A')
			{

			}
			else if (VKCode == 'S') 
			{

			}
			else if (VKCode == 'D') 
			{

			}
			else if (VKCode == 'Q') 
			{

			}
			else if (VKCode == 'E') 
			{

			}
			else if (VKCode == VK_UP) 
			{

			}
			else if (VKCode == VK_LEFT) 
			{

			}
			else if (VKCode == VK_DOWN) 
			{

			}
			else if (VKCode == VK_RIGHT) 
			{

			}
			else if (VKCode == VK_ESCAPE) 
			{
				OutputDebugStringA("ESCAPE");

				if (IsDown) 
				{
					OutputDebugStringA("IsDown");
				}
				if (WasDown)
				{
					OutputDebugStringA("WasDown");
				}
			}
			else if (VKCode == VK_SPACE)
			{

			}
		}

		bool32 AltKeyWasDown = (LParam & (1 << 29));
		if ((VKCode == VK_F4) && AltKeyWasDown) 
		{
			GlobalRunning = false;
		}

	}break;	
	
	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		/*int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;*/
		//local_persist DWORD Operation = WHITENESS;

		win32WindowDimension Dimension = Win32GetWindowDimension(Window);
		
		Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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

struct win32_sound_output
{
	int SamplesPerSecond;
	int  ToneHz;
	int16 ToneVolume;
	uint16 RunningSampleIndex;
	int WavePeriod;
	int BytesPerSample;
	int SecondaryBufferSize;
};

internal void Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite)
{
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;

	if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite, 
											&Region1, &Region1Size, 
											&Region2, &Region2Size, 
											0)))
	{

		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
		int16_t *SampleOut = (int16_t *)Region1;

		for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
		{
			real32 t = 2.0f*Pi32*(real32)SoundOutput->RunningSampleIndex / (real32)SoundOutput->WavePeriod;
			real32 SineValue = sin(t);
			int16 SampleValue = (int16)(SineValue * SoundOutput->ToneVolume);
			//int16 SampleValue = ((RunningSampleIndex / HalfSquareWavePeriod) % 2) ? 6000 : -6000;
			*SampleOut++ = SampleValue;
			*SampleOut++ = SampleValue;
			++SoundOutput->RunningSampleIndex;
		}
		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		SampleOut = (int16 *)Region2;
		for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
		{
			real32 t = 2.0f*Pi32*(real32)SoundOutput->RunningSampleIndex / (real32)SoundOutput->WavePeriod;
			real32 SineValue = sin(t);
			int16 SampleValue = (int16)(SineValue * SoundOutput->ToneVolume);
			*SampleOut++ = SampleValue;
			*SampleOut++ = SampleValue;
			++SoundOutput->RunningSampleIndex;
		}
		GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);


	}
}

int CALLBACK 
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode)
{
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};

	//win32WindowDimension Dimension = Win32GetWindowDimension(Window);
	Win32ResizeDibSection(&GlobalBackBuffer, 1280, 720);

	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeHeroWindowsClass";

	if (RegisterClassA(&WindowClass))
	{
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
		if (Window)
		{
			HDC DeviceContext = GetDC(Window);

			//Graphics
			int XOffSet = 0;
			int YOffSet = 0;


			win32_sound_output SoundOutput = {};

			SoundOutput.SamplesPerSecond = 48000;
			SoundOutput.ToneHz = 256;
			SoundOutput.ToneVolume = 3000;
			SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
			SoundOutput.BytesPerSample = sizeof(int16) * 2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond*SoundOutput.BytesPerSample;
			Win32InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
			Win32FillSoundBuffer(&SoundOutput, 0, SoundOutput.SecondaryBufferSize);
			GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			
			

			bool32 SoundIsPlaying = false;

			GlobalRunning = true;

			while (GlobalRunning)
			{
				MSG Message;
				if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ControllerIndex++)
				{
					XINPUT_STATE ControllerState;
					if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
					{

						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;

						XOffSet += StickX >> 12;
						YOffSet += StickY >> 12;

					}
					else
					{

					}

				}

				RenderWeirdGradient(&GlobalBackBuffer, XOffSet, YOffSet);
				/*
				The following function is a IDirectSoundBuffer8 Lock Method
				https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.idirectsoundbuffer8.idirectsoundbuffer8.lock(v=vs.85).aspx
				*/
				DWORD PlayCursor;
				DWORD WriteCursor;

				if (!SoundIsPlaying &&SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
				{
					DWORD ByteToLock = ((SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize);
					DWORD BytesToWrite;

					if (ByteToLock == PlayCursor)
					{

						BytesToWrite = 0;

					}
					else if(ByteToLock > PlayCursor)
					{
						BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock);
						BytesToWrite += PlayCursor;
					}
					else
					{
						BytesToWrite = PlayCursor - ByteToLock;
					}

					
				}
				win32WindowDimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
			}
		}
		else
		{
			//TODO: Logging
		}
	}

	else
	{
		//TODO: Logging
	}
	return(0);
}
