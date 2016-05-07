#include <thread>
#include <memory>
#include "MessageQueue.h"
#include "RenderService.h"
#include "MultipleCameraService.h"

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
	CoreD3dService* coreService = new CoreD3dService(windowHandle, 1366, 768);
	MultipleCameraService* cameraService = new MultipleCameraService(CameraProperties{ D3DXVECTOR3(0.0f, 3.0f, 5.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f) });
	std::unique_ptr<RenderService> renderService(new RenderService(coreService, cameraService));
	std::thread renderLoopThread([&](){renderService->RenderLoop();});
	messageLoopThread.join();
	renderService->StopService();
	renderLoopThread.join();
	delete cameraService;
	delete coreService;
	return message.wParam;
}

