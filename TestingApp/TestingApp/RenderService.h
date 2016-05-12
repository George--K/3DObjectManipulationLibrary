#pragma once
#include "CoreD3dService.h"
#include "ICameraService.h"
#include "ILightService.h"
#include "ConstantBuffer.h"
#include "RenderObjectManager.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

struct TRANSFORMATION_BUFFER
{
	D3DXMATRIX finalTransformationMatrix;
	D3DXMATRIX rotationMatrix;
};

class RenderService
{
	private:
		volatile bool stopped = false;

		CoreD3dService* coreService;
		RenderObjectManager* objectManager;
		ICameraService* cameraService;
		ILightService* lightService;
		
		ConstantBuffer<TRANSFORMATION_BUFFER>* transformationBuffer;
		void RenderFrame();
		void CleanD3D();
		void InitGraphics();
		void InitPipeline();
	public:
		RenderService(CoreD3dService* coreService, RenderObjectManager* objectManager, ICameraService* cameraService, ILightService* lightService);
		void BeginRenderLoop();
		void StopService();
		~RenderService();
};

