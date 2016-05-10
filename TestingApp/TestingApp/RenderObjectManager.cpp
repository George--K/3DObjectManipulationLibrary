#include "RenderObjectManager.h"
#include <stdexcept>
#include <utility>

std::pair<VERTEX*, DWORD*> DummyLoad(std::wstring meshPath, int* verticesSize, int* indicesSize)
{
	VERTEX* OurVertices = new VERTEX[24]
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

	*verticesSize = 24;

	DWORD* OurIndices = new DWORD[36]
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

	*indicesSize = 36;

	return std::make_pair(OurVertices, OurIndices);
}

RenderObjectManager::RenderObjectManager(CoreD3dService* coreService, MESH_DELETION_TYPE meshPersistence)
{
	this->coreService = coreService;
	this->meshPersistence = meshPersistence;
}

RenderObject* RenderObjectManager::LoadObject(std::wstring meshPath, D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale, std::wstring texturePath)
{
	if (meshMap.count(meshPath) == 0)
	{
		LoadMesh(meshPath, texturePath);
	}
	RenderObject* obj = new RenderObject(meshPath, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z);
	objectMap[meshPath].insert(obj);
	return obj;
}

void RenderObjectManager::UnloadObject(RenderObject* object)
{
	objectMap[object->getMeshPath()].erase(object);
	if (objectMap[object->getMeshPath()].size() == 0 && meshPersistence == AUTO_DELETION)
	{
		UnloadMesh(object->getMeshPath());
	}
	delete object;
}

void RenderObjectManager::LoadMesh(std::wstring meshPath, std::wstring texturePath)
{
	if (meshMap.count(meshPath) != 0)
	{
		UnloadMesh(meshPath);
	}

	ID3D11ShaderResourceView* texture;
	if (texturePath != L"")
	{
		coreService->RegisterTexture(texturePath.c_str(), &texture);
	}
	else
	{
		texture = NULL;
	}

	int numberOfVertices;
	int numberOfIndices;
	std::pair<VERTEX*, DWORD*> meshData = DummyLoad(meshPath, &numberOfVertices, &numberOfIndices);

	ID3D11Buffer* vertexBuffer;
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(bufferDescription));
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = sizeof(VERTEX) * numberOfVertices;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	coreService->CreateBuffer(&bufferDescription, &vertexBuffer);
	coreService->UpdateDynamicBuffer(vertexBuffer, meshData.first, sizeof(VERTEX) * numberOfVertices);

	ID3D11Buffer* indexBuffer;
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = sizeof(DWORD) * numberOfIndices;
	bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;
	coreService->CreateBuffer(&bufferDescription, &indexBuffer);
	coreService->UpdateDynamicBuffer(indexBuffer, meshData.second, sizeof(DWORD) * numberOfIndices);

	meshMap[meshPath] = Mesh { vertexBuffer, indexBuffer, texture };
}

void RenderObjectManager::SetActiveMesh(std::wstring meshPath)
{
	if (meshMap.count(meshPath) == 0)
	{
		throw std::invalid_argument("Mesh not loaded.");
	}
	Mesh mesh = meshMap[meshPath];
	UINT stride = sizeof(VERTEX);
	coreService->SetVertexIndexBuffers(&(mesh.vertexBuffer), &stride, mesh.indexBuffer);
	if (mesh.texture != NULL)
	{
		coreService->SetActiveTexture(&(mesh.texture));
	}
}

void RenderObjectManager::UnloadMesh(std::wstring meshPath)
{
	if (meshMap.count(meshPath) == 0)
	{
		throw std::invalid_argument("Mesh not loaded.");
	}
	meshMap.erase(meshPath);
}

RenderObjectManager::~RenderObjectManager()
{
	for (auto i = objectMap.begin(); i != objectMap.end(); i++)
	{
		for (auto j = i->second.begin(); j != i->second.end(); j++)
		{
			delete *j;
		}
	}
}
