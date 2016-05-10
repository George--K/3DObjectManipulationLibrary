#pragma once
#include "CoreD3dService.h"
#include <map>
#include <string>

class ShaderLoader
{
	private:
		CoreD3dService* coreService;
		std::map<std::wstring, ID3D11VertexShader*> vertexShaders;
		std::map<std::wstring, ID3D11PixelShader*> pixelShaders;
		ID3D11InputLayout* inputLayout;
	public:
		ShaderLoader(CoreD3dService* coreService, std::wstring initialVertexShaderPath, std::wstring initialPixelShaderPath);
		void LoadVertexShader(std::wstring shaderPath);
		void SetActiveVertexShader(std::wstring shaderPath);
		void UnloadVertexShader(std::wstring shaderPath);
		void LoadPixelShader(std::wstring shaderPath);
		void SetActivePixelShader(std::wstring shaderPath);
		void UnloadPixelShader(std::wstring shaderPath);
		~ShaderLoader();
};