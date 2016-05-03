#pragma once
#include "Mesh.h"
#include <map>

class IMeshService
{
	public:
		virtual std::map<std::string, Mesh> getLoadedMeshDictionary() = 0;
		virtual void loadMesh(std::string meshName) = 0;
};