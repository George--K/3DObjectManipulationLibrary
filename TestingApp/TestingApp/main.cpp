#include <thread>
#include <memory>
#include "MessageQueue.h"
#include "D3dRenderService.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND windowHandle;
	MSG message;
	std::thread messageLoopThread(MessageQueue::MessageLoop, &message, hInstance, &windowHandle);
	std::unique_lock<std::mutex> lock(MessageQueue::windowReadyLock);
	while (!MessageQueue::windowReady)
	{
		MessageQueue::windowReadyConditionalVariable.wait(lock);
	}
	ShowWindow(windowHandle, nCmdShow);
	std::unique_ptr<D3dRenderService> renderService(new D3dRenderService(windowHandle, 1366, 768));
	std::thread renderLoopThread([&](){renderService->RenderLoop();});
	messageLoopThread.join();
	renderService->StopService();
	renderLoopThread.join();
	return message.wParam;
}

