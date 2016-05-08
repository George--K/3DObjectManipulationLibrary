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
		ID3D11RenderTargetView* getBackBuffer();
		ID3D11DepthStencilView* getDepthBuffer();
		float getAspectRatio();
		HRESULT CreateBuffer(D3D11_BUFFER_DESC* desc, ID3D11Buffer** buffer);
		void RegisterConstantBuffer(int slot, ID3D11Buffer* const* buffer);
		void UpdateDefaultBuffer(ID3D11Buffer* buffer, const void* data);
		void UpdateDynamicBuffer(ID3D11Buffer* buffer, const void* data, size_t length);
		~CoreD3dService();
};

