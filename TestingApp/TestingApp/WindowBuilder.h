#pragma once
#include <windows.h>
#include <windowsx.h>

namespace WindowBuilder
{
	LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	HWND Create(int width, int height, HINSTANCE hInstance, WNDPROC WinProc = WindowProc);
};

