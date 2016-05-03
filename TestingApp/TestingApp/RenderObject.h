#pragma once
#include "Mesh.h"
#include <string>

class RenderObject
{
	private:
		float locationX, locationY, locationZ;
		float rotationX, rotationY, rotationZ;
		float scaleX, scaleY, scaleZ;
		std::string meshName;
	public:
		RenderObject(std::string meshName,
					 float locationX = 0.0f, float locationY = 0.0f, float locationZ = 0.0f,
					 float rotationX = 0.0f, float rotationY = 0.0f, float rotationZ = 0.0f,
					 float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f);
		void MoveTo(float locationX, float locationY, float locationZ);
		void setRotation(float rotationX, float rotationY, float rotationZ);
		void setScaling(float scaleX, float scaleY, float scaleZ);
		~RenderObject();
};

