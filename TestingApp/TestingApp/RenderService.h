#pragma once
#include "CoreD3dService.h"
#include "ICameraService.h"
#include "ILightService.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

struct MATRICES_BUFFER
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
		
		ID3D11InputLayout *inputLayout;
		ID3D11VertexShader *vertexShader;
		ID3D11PixelShader *pixelShader;
		ID3D11Buffer *vertexBuffer;
		ID3D11Buffer *indexBuffer;
		ID3D11Buffer *matricesBuffer;
		ID3D11ShaderResourceView *texture;

		ID3D11RasterizerState *rasterizerState;
		ID3D11SamplerState *samplerState;
		ID3D11BlendState *blendState;

		void RenderFrame();
		void CleanD3D();
		void InitGraphics();
		void InitPipeline();
		void InitStates();
	public:
		RenderService(CoreD3dService* coreService, ICameraService* cameraService, ILightService* lightService);
		void BeginRenderLoop();
		void StopService();
		~RenderService();
};

