#pragma once
#include <windows.h>
#include <windowsx.h>
#include <mutex>
#include <condition_variable>
#include "WindowBuilder.h"

namespace MessageQueue
{
	std::mutex windowReadyLock;
	std::condition_variable windowReadyConditionalVariable;
	bool windowReady;

	void MessageLoop(MSG* message, HINSTANCE hInstance, HWND* windowHandle)
	{
		*(windowHandle) = WindowBuilder::Create(1366, 768, hInstance);
		windowReady = true;
		windowReadyConditionalVariable.notify_all();
		while (TRUE)
		{
			if (PeekMessage(message, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(message);
				DispatchMessage(message);
				if (message->message == WM_QUIT)
					break;
			}
		}
	}
}