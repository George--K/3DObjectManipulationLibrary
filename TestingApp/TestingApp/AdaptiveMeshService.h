#pragma once
#include <fstream>
#include <map>
#include "Mesh.h"

class AdaptiveMeshService
{
	private:
		std::map<std::string, Mesh> loadedMeshDictionary;
		void loadMeshFromObj(std::ifstream stream);
		void loadMeshFromBlend(std::ifstream stream);
		void loadMeshFromMa(std::ifstream stream);
		void loadMeshFrom3ds(std::ifstream stream);
	public:
		AdaptiveMeshService();
		virtual std::map<std::string, Mesh> getLoadedMeshDictionary();
		virtual void loadMesh(std::string meshName);
		~AdaptiveMeshService();
};

