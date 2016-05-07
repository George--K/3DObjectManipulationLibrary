#include "CoreD3dService.h"


CoreD3dService::CoreD3dService(HWND windowHandle, int screenWidth, int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescription.BufferCount = 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.Width = screenWidth;
	swapChainDescription.BufferDesc.Height = screenHeight;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.OutputWindow = windowHandle;
	swapChainDescription.SampleDesc.Count = 4;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&swapChainDescription,
		&swapChain,
		&d3dDeviceInterface,
		NULL,
		&d3dDeviceContext);

	D3D11_TEXTURE2D_DESC textureDescription;
	ZeroMemory(&textureDescription, sizeof(textureDescription));

	textureDescription.Width = screenWidth;
	textureDescription.Height = screenHeight;
	textureDescription.ArraySize = 1;
	textureDescription.MipLevels = 1;
	textureDescription.SampleDesc.Count = 4;
	textureDescription.Format = DXGI_FORMAT_D32_FLOAT;
	textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *textureDepthBuffer;
	d3dDeviceInterface->CreateTexture2D(&textureDescription, NULL, &textureDepthBuffer);
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));
	depthStencilViewDescription.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	d3dDeviceInterface->CreateDepthStencilView(textureDepthBuffer, &depthStencilViewDescription, &depthBuffer);
	d3dDeviceInterface->Release();
	ID3D11Texture2D *textureBackBuffer;
	swapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&textureBackBuffer);

	d3dDeviceInterface->CreateRenderTargetView(textureBackBuffer, NULL, &backBuffer);
	d3dDeviceContext->OMSetRenderTargets(1, &backBuffer, depthBuffer);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	d3dDeviceContext->RSSetViewports(1, &viewport);
}

IDXGISwapChain* CoreD3dService::getSwapChain()
{
	return swapChain;
}

ID3D11Device* CoreD3dService::getDeviceInterface()
{
	return d3dDeviceInterface;
}

ID3D11DeviceContext* CoreD3dService::getDeviceContext()
{
	return d3dDeviceContext;
}

float CoreD3dService::getAspectRatio()
{
	return (FLOAT)screenWidth / (FLOAT)screenHeight;
}

CoreD3dService::~CoreD3dService()
{
	depthBuffer->Release();
	backBuffer->Release();
	swapChain->Release();
	d3dDeviceInterface->Release();
	d3dDeviceContext->Release();
}
