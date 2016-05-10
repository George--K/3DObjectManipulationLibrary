#pragma once
#include <string>

class RenderObject
{
	private:
		float locationX, locationY, locationZ;
		float rotationX, rotationY, rotationZ;
		float scaleX, scaleY, scaleZ;
		std::wstring meshPath;
	public:
		RenderObject(std::wstring meshPath,
					 float locationX = 0.0f, float locationY = 0.0f, float locationZ = 0.0f,
					 float rotationX = 0.0f, float rotationY = 0.0f, float rotationZ = 0.0f,
					 float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f);
		std::wstring getMeshPath();
		void MoveTo(float locationX, float locationY, float locationZ);
		void RotateTo(float rotationX, float rotationY, float rotationZ);
		void ScaleTo(float scaleX, float scaleY, float scaleZ);
		~RenderObject();
};

