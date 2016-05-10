#pragma once
#include "CoreD3dService.h"
#include "ICameraService.h"
#include "ILightService.h"
#include "ConstantBuffer.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

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
		ICameraService* cameraService;
		ILightService* lightService;
		
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		ConstantBuffer<TRANSFORMATION_BUFFER>* transformationBuffer;
		ID3D11ShaderResourceView* texture;

		void RenderFrame();
		void CleanD3D();
		void InitGraphics();
		void InitPipeline();
	public:
		RenderService(CoreD3dService* coreService, ICameraService* cameraService, ILightService* lightService);
		void BeginRenderLoop();
		void StopService();
		~RenderService();
};

