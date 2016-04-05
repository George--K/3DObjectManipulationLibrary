#include "WindowBuilder.h"

HWND WindowBuilder::Create(int width, int height, HINSTANCE hInstance, WNDPROC WinProc)
{
	HWND windowHandle;

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	RegisterClassEx(&windowClass);

	RECT windowRectangle = { 0, 0, width, height };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	windowHandle = CreateWindowExW(NULL,
		L"WindowClass",
		L"Direct3D Testing App",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	return windowHandle;
}

LRESULT CALLBACK WindowBuilder::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	}
	return DefWindowProc(windowHandle, message, wParam, lParam);
}