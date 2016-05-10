#include "RenderService.h"

RenderService::RenderService(CoreD3dService* coreService, ICameraService* cameraService, ILightService* lightService)
{
	this->coreService = coreService;
	this->cameraService = cameraService;
	this->lightService = lightService;
	InitPipeline();
	InitGraphics();
	coreService->InitStates();
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
	ID3D11DeviceContext* d3dDeviceContext = coreService->getDeviceContext();

	TRANSFORMATION_BUFFER transformations;

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

	transformations.finalTransformationMatrix = matRotate * matView * matProjection;
	transformations.rotationMatrix = matRotate;

	coreService->SetStates();
	coreService->ClearRenderBuffers();

	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	transformationBuffer->Update(&transformations);
	d3dDeviceContext->PSSetShaderResources(0, 1, &texture);
	d3dDeviceContext->DrawIndexed(36, 0, 0);

	D3DXMatrixTranslation(&matTranslate, 0.0f, 0.0f, -5.0f);
	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 2.0f);
	transformations.finalTransformationMatrix = matRotate * matTranslate * matScale * matView * matProjection;
	transformationBuffer->Update(&transformations);
	lightService->UpdateLightBuffers();
	d3dDeviceContext->DrawIndexed(36, 0, 0);

	coreService->ShowFrame();
}

void RenderService::CleanD3D(void)
{
	coreService->SwitchToWindowedMode();

	delete transformationBuffer;
	vertexBuffer->Release();
	indexBuffer->Release();
	texture->Release();
}

void RenderService::InitGraphics()
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

	coreService->CreateBuffer(&bufferDescription, &vertexBuffer);
	coreService->UpdateDynamicBuffer(vertexBuffer, OurVertices, sizeof(OurVertices));

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

	coreService->CreateBuffer(&bufferDescription, &indexBuffer);
	coreService->UpdateDynamicBuffer(indexBuffer, OurIndices, sizeof(OurIndices));
}

void RenderService::InitPipeline()
{
	HRESULT res = D3DX11CreateShaderResourceViewFromFile(coreService->getDeviceInterface(),
		L"E:\\bricks.png",
		NULL,
		NULL,
		&texture,
		NULL);

	transformationBuffer = new ConstantBuffer<TRANSFORMATION_BUFFER>(16 * 4 * 2, coreService);
}

RenderService::~RenderService()
{
	CleanD3D();
}
