#include "ShaderLoader.h"
#include <stdexcept>


ShaderLoader::ShaderLoader(CoreD3dService* coreService, std::wstring initialVertexShaderPath, std::wstring initialPixelShaderPath)
{
	this->coreService = coreService;
	D3D11_INPUT_ELEMENT_DESC inputElementDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	coreService->SetNewVertexShaderAndInputLayout(initialVertexShaderPath.c_str(), inputElementDescription, 3, &(vertexShaders[initialVertexShaderPath]), &inputLayout);
	SetActivePixelShader(initialPixelShaderPath);
}

void ShaderLoader::LoadVertexShader(std::wstring shaderPath)
{
	if (vertexShaders.count(shaderPath) != 0)
	{
		UnloadVertexShader(shaderPath);
	}
	vertexShaders[shaderPath] = coreService->CreateVertexShader(shaderPath.c_str());
}

void ShaderLoader::SetActiveVertexShader(std::wstring shaderPath)
{
	if (vertexShaders.count(shaderPath) == 0)
	{
		LoadVertexShader(shaderPath);
	}
	coreService->EnableVertexShader(vertexShaders[shaderPath]);
}

void ShaderLoader::UnloadVertexShader(std::wstring shaderPath)
{
	if (vertexShaders.count(shaderPath) == 0)
	{
		throw std::invalid_argument("Vertex shader not loaded");
	}
	vertexShaders[shaderPath]->Release();
	vertexShaders.erase(shaderPath);
}

void ShaderLoader::LoadPixelShader(std::wstring shaderPath)
{
	if (pixelShaders.count(shaderPath) != 0)
	{
		UnloadPixelShader(shaderPath);
	}
	pixelShaders[shaderPath] = coreService->CreatePixelShader(shaderPath.c_str());
}

void ShaderLoader::SetActivePixelShader(std::wstring shaderPath)
{
	if (pixelShaders.count(shaderPath) == 0)
	{
		LoadPixelShader(shaderPath);
	}
	coreService->EnablePixelShader(pixelShaders[shaderPath]);
}

void ShaderLoader::UnloadPixelShader(std::wstring shaderPath)
{
	if (pixelShaders.count(shaderPath) == 0)
	{
		throw std::invalid_argument("Pixel shader not loaded");
	}
	pixelShaders[shaderPath]->Release();
	pixelShaders.erase(shaderPath);
}

ShaderLoader::~ShaderLoader()
{
	for (auto i = vertexShaders.begin(); i != vertexShaders.end(); i++)
	{
		i->second->Release();
	}

	for (auto i = pixelShaders.begin(); i != pixelShaders.end(); i++)
	{
		i->second->Release();
	}
	inputLayout->Release();
}
