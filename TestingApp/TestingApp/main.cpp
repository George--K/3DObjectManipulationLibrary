#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_WIDTH  1366
#define SCREEN_HEIGHT 768

IDXGISwapChain *swapÑhain;
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

struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

struct CONSTANT_BUFFER
{
	D3DXMATRIX finalTransformationMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXVECTOR4 directionalLightVector;
	D3DXCOLOR directionalLightColor;
	D3DXCOLOR ambientLightColor;
};

void InitD3D(HWND hWnd);
void RenderFrame(void);
void CleanD3D(void);
void InitGraphics(void);
void InitPipeline(void);
void InitStates(void);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND windowHandle;
	WNDCLASSEX windowClass;

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";

	RegisterClassEx(&windowClass);

	RECT windowRectangle = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	windowHandle = CreateWindowExW(NULL,
		L"WindowClass",
		L"Direct3D Testing App",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(windowHandle, nCmdShow);
	InitD3D(windowHandle);

	MSG message;
	while (TRUE)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
			if (message.message == WM_QUIT)
				break;
		}
		RenderFrame();
	}

	CleanD3D();
	return message.wParam;
}

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
				{
					PostQuitMessage(0);
					return 0;
				}
				break;
	}
	return DefWindowProc(windowHandle, message, wParam, lParam);
}

void InitD3D(HWND windowHandle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescription.BufferCount = 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDescription.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.OutputWindow = windowHandle;
	swapChainDescription.SampleDesc.Count = 4;
	swapChainDescription.Windowed = FALSE;
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

	textureDescription.Width = SCREEN_WIDTH;
	textureDescription.Height = SCREEN_HEIGHT;
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
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	d3dDeviceContext->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics();
	InitStates();
}

void RenderFrame(void)
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
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
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

void CleanD3D(void)
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

void InitGraphics()
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

void InitPipeline()
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

void InitStates()
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