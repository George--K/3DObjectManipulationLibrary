#include "RenderService.h"

RenderService::RenderService(CoreD3dService* coreService, ICameraService* cameraService, ILightService* lightService)
{
	this->coreService = coreService;
	this->cameraService = cameraService;
	this->lightService = lightService;
	InitPipeline();
	InitGraphics();
	InitStates();
}

void RenderService::BeginRenderLoop()
{
	while (!stopped)
	{
		RenderFrame();
		Sleep(0);
	}
}

void RenderService::StopService()
{
	stopped = true;
}

void RenderService::RenderFrame(void)
{
	ID3D11Device* d3dDeviceInterface = coreService->getDeviceInterface();
	ID3D11DeviceContext* d3dDeviceContext = coreService->getDeviceContext();

	MATRICES_BUFFER constantBuffer;

	D3DXMATRIX matTranslate, matRotate, matScale, matView, matProjection;
	D3DXMATRIX matFinal;

	D3DXMatrixRotationY(&matRotate, 0.5);

	CameraProperties cameraProperties = cameraService->getCameraPropertiesForRendering();

	D3DXMatrixLookAtLH(&matView,
		&cameraProperties.Location,
		&cameraProperties.LookAt,
		&cameraProperties.UpDirection);

	D3DXMatrixPerspectiveFovLH(&matProjection,
		(FLOAT)D3DXToRadian(45),
		coreService->getAspectRatio(),
		1.0f,
		100.0f);

	constantBuffer.finalTransformationMatrix = matRotate * matView * matProjection;
	constantBuffer.rotationMatrix = matRotate;

	d3dDeviceContext->RSSetState(rasterizerState);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	d3dDeviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
	d3dDeviceContext->ClearRenderTargetView(coreService->getBackBuffer(), D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
	d3dDeviceContext->ClearDepthStencilView(coreService->getDepthBuffer(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dDeviceContext->UpdateSubresource(matricesBuffer, NULL, NULL, &constantBuffer, 0, 0);
	d3dDeviceContext->PSSetShaderResources(0, 1, &texture);
	d3dDeviceContext->DrawIndexed(36, 0, 0);
	D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, -5.0f);
	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 2.0f);
	constantBuffer.finalTransformationMatrix = matRotate * matTranslate * matScale * matView * matProjection;
	d3dDeviceContext->UpdateSubresource(matricesBuffer, NULL, NULL, &constantBuffer, 0, 0);
	lightService->UpdateLightBuffers();
	d3dDeviceContext->DrawIndexed(36, 0, 0);
	coreService->getSwapChain()->Present(0, NULL);
}

void RenderService::CleanD3D(void)
{
	coreService->getSwapChain()->SetFullscreenState(FALSE, NULL);
	inputLayout->Release();
	vertexShader->Release();
	pixelShader->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	matricesBuffer->Release();
	texture->Release();
}

void RenderService::InitGraphics()
{
	ID3D11Device* d3dDeviceInterface = coreService->getDeviceInterface();
	ID3D11DeviceContext* d3dDeviceContext = coreService->getDeviceContext();

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

void RenderService::InitPipeline()
{
	ID3D11Device* d3dDeviceInterface = coreService->getDeviceInterface();
	ID3D11DeviceContext* d3dDeviceContext = coreService->getDeviceContext();

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
	buffDesc.ByteWidth = sizeof(MATRICES_BUFFER);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dDeviceInterface->CreateBuffer(&buffDesc, NULL, &matricesBuffer);
	d3dDeviceContext->VSSetConstantBuffers(1, 1, &matricesBuffer);
}

void RenderService::InitStates()
{
	ID3D11Device* d3dDeviceInterface = coreService->getDeviceInterface();

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

RenderService::~RenderService()
{
	CleanD3D();
}
