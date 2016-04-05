#pragma once
#include <windows.h>
#include <windowsx.h>

static class WindowBuilder
{
	public:
		static HWND Create(int width, int height, HINSTANCE hInstance, WNDPROC WinProc = WindowProc);
		static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
};

