#include <thread>
#include "3DObjectManipulationLibrary.h"

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
	SimpleLightService* lightService = new SimpleLightService(coreService, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f), D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f));
	ShaderLoader* shaderLoader = new ShaderLoader(coreService, L"shaders.shader", L"shaders.shader");
	RenderObjectManager* rom = new RenderObjectManager(coreService);
	RenderService* renderService = new RenderService(coreService, rom, cameraService, lightService);
	rom->LoadObject(L"test.obj", D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.5f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), L"bricks.png");
	std::thread renderLoopThread([&](){renderService->BeginRenderLoop();});
	messageLoopThread.join();
	renderService->StopService();
	renderLoopThread.join();
	delete renderService;
	delete rom;
	delete shaderLoader;
	delete lightService;
	delete cameraService;
	delete coreService;
	return message.wParam;
}

