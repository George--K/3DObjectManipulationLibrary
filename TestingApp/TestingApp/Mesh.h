#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

class Mesh
{
	private:
		ID3D11Buffer *vertexBuffer;
		ID3D11Buffer *indexBuffer;
		std::string textureName;
	public:
		Mesh();
		~Mesh();
};