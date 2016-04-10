#include "D3dRenderService.h"


D3dRenderService::D3dRenderService(HWND windowHandle, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	InitD3D(windowHandle);
}

void D3dRenderService::RenderLoop()
{
	while (!stopped)
	{
		RenderFrame();
		Sleep(0);
	}
}

void D3dRenderService::StopService()
{
	stopped = true;
}

void D3dRenderService::InitD3D(HWND windowHandle)
{
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
		&swapÑhain,
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
	swapÑhain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&textureBackBuffer);

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

	InitPipeline();
	InitGraphics();
	InitStates();
}

void D3dRenderService::RenderFrame(void)
{
	CONSTANT_BUFFER constantBuffer;

	constantBuffer.directionalLightVector = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	constantBuffer.directionalLightColor = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	constantBuffer.ambientLightColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	D3DXMATRIX matRotate, matView, matProjection;
	D3DXMATRIX matFinal;

	static float Time = 0.0f; Time += 0.0003f;
	D3DXMatrixRotationY(&matRotate, Time);

	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 3.0f, 5.0f),
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixPerspectiveFovLH(&matProjection,
		(FLOAT)D3DXToRadian(45),
		(FLOAT)screenWidth / (FLOAT)screenHeight,
		1.0f,
		100.0f);

	constantBuffer.finalTransformationMatrix = matRotate * matView * matProjection;
	constantBuffer.rotationMatrix = matRotate;

	d3dDeviceContext->RSSetState(rasterizerState);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	d3dDeviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
	d3dDeviceContext->ClearRenderTargetView(backBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
	d3dDeviceContext->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dDeviceContext->UpdateSubresource(constBuffer, NULL, NULL, &constantBuffer, 0, 0);
	d3dDeviceContext->PSSetShaderResources(0, 1, &texture);
	d3dDeviceContext->DrawIndexed(36, 0, 0);

	swapÑhain->Present(0, NULL);
}

void D3dRenderService::CleanD3D(void)
{
	swapÑhain->SetFullscreenState(FALSE, NULL);

	depthBuffer->Release();
	inputLayout->Release();
	vertexShader->Release();
	pixelShader->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	constBuffer->Release();
	texture->Release();
	swapÑhain->Release();
	backBuffer->Release();
	d3dDeviceInterface->Release();
	d3dDeviceContext->Release();
}

void D3dRenderService::InitGraphics()
{
	VERTEX OurVertices[] =
	{
		{ -1.0f, -1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f },
		{ -1.0f, 1.0f, -1.0f, D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f, D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f, D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f },

		{ -1.0f, 1.0f, -1.0f, D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f },
		{ -1.0f, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f, D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f },
		{ 1.0f, -1.0f, -1.0f, D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f, D3DXVECTOR3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, D3DXVECTOR3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f },

		{ 1.0f, -1.0f, -1.0f, D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f, D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f, D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f, D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f },
		{ -1.0f, -1.0f, 1.0f, D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f, D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f },
		{ -1.0f, 1.0f, 1.0f, D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
	};

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));

	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = sizeof(VERTEX) * 24;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	d3dDeviceInterface->CreateBuffer(&bufferDescription, NULL, &vertexBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	d3dDeviceContext->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, OurVertices, sizeof(OurVertices));
	d3dDeviceContext->Unmap(vertexBuffer, NULL);

	DWORD OurIndices[] =
	{
		0, 1, 2,
		2, 1, 3,
		4, 5, 6,
		6, 5, 7,
		8, 9, 10,
		10, 9, 11,
		12, 13, 14,
		14, 13, 15,
		16, 17, 18,
		18, 17, 19,
		20, 21, 22,
		22, 21, 23,
	};

	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = sizeof(DWORD) * 36;
	bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;

	d3dDeviceInterface->CreateBuffer(&bufferDescription, NULL, &indexBuffer);

	d3dDeviceContext->Map(indexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, OurIndices, sizeof(OurIndices));
	d3dDeviceContext->Unmap(indexBuffer, NULL);

	D3DX11CreateShaderResourceViewFromFile(d3dDeviceInterface,
		L"E:\\bricks.png",
		NULL,
		NULL,
		&texture,
		NULL);
}

void D3dRenderService::InitPipeline()
{
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	d3dDeviceInterface->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vertexShader);
	d3dDeviceInterface->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pixelShader);
	d3dDeviceContext->VSSetShader(vertexShader, 0, 0);
	d3dDeviceContext->PSSetShader(pixelShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC inputElementDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	d3dDeviceInterface->CreateInputLayout(inputElementDescription, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &inputLayout);
	d3dDeviceContext->IASetInputLayout(inputLayout);

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.ByteWidth = 176;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dDeviceInterface->CreateBuffer(&buffDesc, NULL, &constBuffer);
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &constBuffer);
}

void D3dRenderService::InitStates()
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

D3dRenderService::~D3dRenderService()
{
	CleanD3D();
}
