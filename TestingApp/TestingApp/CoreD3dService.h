#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

class CoreD3dService
{
	private:
		IDXGISwapChain *swapChain;
		ID3D11Device *d3dDeviceInterface;
		ID3D11DeviceContext *d3dDeviceContext;
		ID3D11RenderTargetView *backBuffer;
		ID3D11DepthStencilView *depthBuffer;
		int screenWidth;
		int screenHeight;
	public:
		CoreD3dService(HWND windowHandle, int screenWidth, int screenHeight);
		IDXGISwapChain* getSwapChain();
		ID3D11Device* getDeviceInterface();
		ID3D11DeviceContext* getDeviceContext();
		float getAspectRatio();
		~CoreD3dService();
};

