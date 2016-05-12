#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

class CoreD3dService
{
	private:
		IDXGISwapChain *swapChain;
		ID3D11Device *d3dDeviceInterface;
		ID3D11DeviceContext *d3dDeviceContext;
		ID3D11RasterizerState* rasterizerState;
		ID3D11SamplerState* samplerState;
		ID3D11BlendState* blendState;
		ID3D11RenderTargetView *backBuffer;
		ID3D11DepthStencilView *depthBuffer;
		int screenWidth;
		int screenHeight;
	public:
		CoreD3dService(HWND windowHandle, int screenWidth, int screenHeight);
		void InitStates();
		void SetStates();
		HRESULT ShowFrame();
		HRESULT SwitchToFullscreenMode();
		HRESULT SwitchToWindowedMode();
		void ClearRenderBuffers();
		float GetAspectRatio();
		void Draw(int numberOfIndices);
		HRESULT CreateBuffer(D3D11_BUFFER_DESC* desc, ID3D11Buffer** buffer);
		void RegisterConstantBuffer(int slot, ID3D11Buffer* const* buffer);
		void UpdateDefaultBuffer(ID3D11Buffer* buffer, const void* data);
		void UpdateDynamicBuffer(ID3D11Buffer* buffer, const void* data, size_t length);
		void SetVertexIndexBuffers(ID3D11Buffer* const* vertexBuffer, const UINT* stride, ID3D11Buffer* indexBuffer);
		HRESULT RegisterTexture(LPCWSTR path, ID3D11ShaderResourceView** texture);
		void SetActiveTexture(ID3D11ShaderResourceView* const* texture);
		void SetNewVertexShaderAndInputLayout(LPCWSTR path, D3D11_INPUT_ELEMENT_DESC* inputDescription, UINT numElements, ID3D11VertexShader** outShader, ID3D11InputLayout** outLayout);
		ID3D11VertexShader* CreateVertexShader(LPCWSTR path);
		ID3D11PixelShader* CreatePixelShader(LPCWSTR path);
		void EnableVertexShader(ID3D11VertexShader* shader);
		void EnablePixelShader(ID3D11PixelShader* shader);
		~CoreD3dService();
};

