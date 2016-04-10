#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include "ICameraService.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

struct CONSTANT_BUFFER
{
	D3DXMATRIX finalTransformationMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXVECTOR4 directionalLightVector;
	D3DXCOLOR directionalLightColor;
	D3DXCOLOR ambientLightColor;
};

class D3dRenderService
{
	private:
		bool stopped = false;

		ICameraService* cameraService;

		int screenWidth;
		int screenHeight;

		IDXGISwapChain *swap—hain;
		ID3D11Device *d3dDeviceInterface;
		ID3D11DeviceContext *d3dDeviceContext;
		ID3D11RenderTargetView *backBuffer;
		ID3D11DepthStencilView *depthBuffer;
		ID3D11InputLayout *inputLayout;
		ID3D11VertexShader *vertexShader;
		ID3D11PixelShader *pixelShader;
		ID3D11Buffer *vertexBuffer;
		ID3D11Buffer *indexBuffer;
		ID3D11Buffer *constBuffer;
		ID3D11ShaderResourceView *texture;

		ID3D11RasterizerState *rasterizerState;
		ID3D11SamplerState *samplerState;
		ID3D11BlendState *blendState;
	public:
		D3dRenderService(HWND windowHandle, int screenWidth, int screenHeight, ICameraService* cameraService);
		void RenderLoop();
		void StopService();
		void InitD3D(HWND windowHandle);
		void RenderFrame();
		void CleanD3D();
		void InitGraphics();
		void InitPipeline();
		void InitStates();
		~D3dRenderService();
};

