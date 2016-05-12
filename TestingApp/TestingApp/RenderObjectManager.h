#pragma once
#include <map>
#include <set>
#include <string>
#include "RenderObject.h"
#include "Mesh.h"
#include "CoreD3dService.h"

struct VERTEX { FLOAT X, Y, Z; D3DXVECTOR3 Normal; FLOAT U, V; };

enum MESH_DELETION_TYPE {
	AUTO_DELETION,
	MANUAL_DELETION
};

class RenderObjectManager
{
	private:
		std::map<std::wstring, std::set<RenderObject*>> objectMap;
		std::map<std::wstring, Mesh*> meshMap;
		std::set<std::wstring> meshSet;
		MESH_DELETION_TYPE meshPersistence;
		CoreD3dService* coreService;
	public:
		RenderObjectManager(CoreD3dService* coreService, MESH_DELETION_TYPE meshPersistence = AUTO_DELETION);
		RenderObject* LoadObject(std::wstring meshPath, D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale, std::wstring texturePath = L"");
		void UnloadObject(RenderObject* object);
		void LoadMesh(std::wstring meshPath, std::wstring texturePath = L"");
		void UnloadMesh(std::wstring meshPath);
		void SetActiveMesh(std::wstring meshPath, int* outNumberOfIndices);
		std::set<std::wstring> GetMeshes();
		std::set<RenderObject*> GetObjectsForMesh(std::wstring meshPath);
		~RenderObjectManager();
};

