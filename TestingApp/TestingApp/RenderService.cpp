#include "RenderService.h"

RenderService::RenderService(CoreD3dService* coreService, RenderObjectManager* objectManager, ICameraService* cameraService, ILightService* lightService)
{
	this->coreService = coreService;
	this->objectManager = objectManager;
	this->cameraService = cameraService;
	this->lightService = lightService;
	transformationBuffer = new ConstantBuffer<TRANSFORMATION_BUFFER>(16 * 4 * 2, coreService);
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
	D3DXMATRIX matView, matProjection;

	CameraProperties cameraProperties = cameraService->getCameraPropertiesForRendering();
	D3DXMatrixLookAtLH(&matView,
		&cameraProperties.Location,
		&cameraProperties.LookAt,
		&cameraProperties.UpDirection);

	D3DXMatrixPerspectiveFovLH(&matProjection,
		(FLOAT)D3DXToRadian(45),
		coreService->GetAspectRatio(),
		1.0f,
		100.0f);

	coreService->SetStates();
	coreService->ClearRenderBuffers();
	lightService->UpdateLightBuffers();

	std::set<std::wstring> meshIds = objectManager->GetMeshes();
	for (auto i = meshIds.begin(); i != meshIds.end(); i++)
	{
		int numberOfIndices;
		objectManager->SetActiveMesh(*i, &numberOfIndices);
		std::set<RenderObject*> objects = objectManager->GetObjectsForMesh(*i);
		for (auto j = objects.begin(); j != objects.end(); j++)
		{
			TRANSFORMATION_BUFFER transformations;
			D3DXMATRIX matTranslate;
			auto location = (*j)->GetPosition();
			D3DXMatrixTranslation(&matTranslate, std::get<0>(location), std::get<1>(location), std::get<2>(location));
			D3DXMATRIX matRotate;
			auto rotation = (*j)->GetRotation();
			D3DXMatrixRotationYawPitchRoll(&matRotate, std::get<0>(rotation), std::get<1>(rotation), std::get<2>(rotation));
			D3DXMATRIX matScale;
			auto scale = (*j)->GetScale();
			D3DXMatrixScaling(&matScale, std::get<0>(scale), std::get<1>(scale), std::get<2>(scale));
			D3DXMATRIX matFinal = matScale * matRotate * matTranslate * matView * matProjection;
			transformations.finalTransformationMatrix = matFinal;
			transformations.rotationMatrix = matRotate;
			transformationBuffer->Update(&transformations);
			coreService->Draw(numberOfIndices);
		}
	}
	coreService->ShowFrame();
}

void RenderService::CleanD3D(void)
{
	coreService->SwitchToWindowedMode();
	delete transformationBuffer;
}

RenderService::~RenderService()
{
	CleanD3D();
}
