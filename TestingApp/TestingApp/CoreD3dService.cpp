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
	textureDepthBuffer->Release();
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

void CoreD3dService::InitStates()
{
	D3D11_RASTERIZER_DESC rasterizerDescription;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FrontCounterClockwise = FALSE;
	rasterizerDescription.DepthClipEnable = TRUE;
	rasterizerDescription.ScissorEnable = FALSE;
	rasterizerDescription.AntialiasedLineEnable = FALSE;
	rasterizerDescription.MultisampleEnable = FALSE;
	rasterizerDescription.DepthBias = 0;
	rasterizerDescription.DepthBiasClamp = 0.0f;
	rasterizerDescription.SlopeScaledDepthBias = 0.0f;
	d3dDeviceInterface->CreateRasterizerState(&rasterizerDescription, &rasterizerState);

	D3D11_SAMPLER_DESC samplerDescription;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.MaxAnisotropy = 16;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.BorderColor[0] = 0.0f;
	samplerDescription.BorderColor[1] = 0.0f;
	samplerDescription.BorderColor[2] = 0.0f;
	samplerDescription.BorderColor[3] = 0.0f;
	samplerDescription.MinLOD = 0.0f;
	samplerDescription.MaxLOD = FLT_MAX;
	samplerDescription.MipLODBias = 0.0f;
	d3dDeviceInterface->CreateSamplerState(&samplerDescription, &samplerState);

	D3D11_BLEND_DESC blendDescription;
	blendDescription.RenderTarget[0].BlendEnable = TRUE;
	blendDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDescription.IndependentBlendEnable = FALSE;
	blendDescription.AlphaToCoverageEnable = FALSE;

	d3dDeviceInterface->CreateBlendState(&blendDescription, &blendState);
}

void CoreD3dService::SetStates()
{
	d3dDeviceContext->RSSetState(rasterizerState);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	d3dDeviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
}
HRESULT CoreD3dService::ShowFrame()
{
	return swapChain->Present(0, NULL);
}

HRESULT CoreD3dService::SwitchToFullscreenMode()
{
	return swapChain->SetFullscreenState(TRUE, NULL);
}

HRESULT CoreD3dService::SwitchToWindowedMode()
{
	return swapChain->SetFullscreenState(FALSE, NULL);
}

ID3D11Device* CoreD3dService::getDeviceInterface()
{
	return d3dDeviceInterface;
}

ID3D11DeviceContext* CoreD3dService::getDeviceContext()
{
	return d3dDeviceContext;
}

void CoreD3dService::ClearRenderBuffers()
{
	d3dDeviceContext->ClearRenderTargetView(backBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
	d3dDeviceContext->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

float CoreD3dService::getAspectRatio()
{
	return (FLOAT)screenWidth / (FLOAT)screenHeight;
}

HRESULT CoreD3dService::CreateBuffer(D3D11_BUFFER_DESC* desc, ID3D11Buffer** buffer)
{
	return d3dDeviceInterface->CreateBuffer(desc, NULL, buffer);
}

void CoreD3dService::RegisterConstantBuffer(int slot, ID3D11Buffer* const* buffer)
{
	d3dDeviceContext->VSSetConstantBuffers(slot, 1, buffer);
}

void CoreD3dService::UpdateDefaultBuffer(ID3D11Buffer* buffer, const void* data)
{
	d3dDeviceContext->UpdateSubresource(buffer, NULL, NULL, data, 0, 0);
}

void CoreD3dService::UpdateDynamicBuffer(ID3D11Buffer* buffer, const void* data, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	d3dDeviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, data, size);
	d3dDeviceContext->Unmap(buffer, NULL);
}

void CoreD3dService::SetVertexIndexBuffers(ID3D11Buffer* const* vertexBuffer, const UINT* stride, ID3D11Buffer* indexBuffer)
{
	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer, stride, &offset);
	d3dDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

HRESULT CoreD3dService::RegisterTexture(LPCWSTR path, ID3D11ShaderResourceView** texture)
{
	return D3DX11CreateShaderResourceViewFromFile(d3dDeviceInterface,
		path,
		NULL,
		NULL,
		texture,
		NULL);
}

void CoreD3dService::SetActiveTexture(ID3D11ShaderResourceView* const* texture)
{
	d3dDeviceContext->PSSetShaderResources(0, 1, texture);
}

void CoreD3dService::SetNewVertexShaderAndInputLayout(LPCWSTR path, D3D11_INPUT_ELEMENT_DESC* inputDescription, UINT numElements, ID3D11VertexShader** outShader, ID3D11InputLayout** outLayout)
{
	ID3D10Blob *VS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	ID3D11VertexShader* vertexShader;
	d3dDeviceInterface->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vertexShader);
	EnableVertexShader(vertexShader);
	*outShader = vertexShader;
	ID3D11InputLayout* inputLayout;
	d3dDeviceInterface->CreateInputLayout(inputDescription, numElements, VS->GetBufferPointer(), VS->GetBufferSize(), &inputLayout);
	d3dDeviceContext->IASetInputLayout(inputLayout);
	*outLayout = inputLayout;
}

ID3D11VertexShader* CoreD3dService::CreateVertexShader(LPCWSTR path)
{
	ID3D10Blob *VS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	ID3D11VertexShader* vertexShader;
	d3dDeviceInterface->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vertexShader);
	return vertexShader;
}

ID3D11PixelShader* CoreD3dService::CreatePixelShader(LPCWSTR path)
{
	ID3D10Blob *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);
	ID3D11PixelShader* pixelShader;
	HRESULT res = d3dDeviceInterface->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pixelShader);
	return pixelShader;
}

void CoreD3dService::EnableVertexShader(ID3D11VertexShader* shader)
{
	d3dDeviceContext->VSSetShader(shader, 0, 0);
}

void CoreD3dService::EnablePixelShader(ID3D11PixelShader* shader)
{
	d3dDeviceContext->PSSetShader(shader, 0, 0);
}

CoreD3dService::~CoreD3dService()
{
	depthBuffer->Release();
	backBuffer->Release();
	swapChain->Release();
	d3dDeviceInterface->Release();
	d3dDeviceContext->Release();
}
