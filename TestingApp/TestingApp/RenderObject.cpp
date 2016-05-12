#include "RenderObject.h"


RenderObject::RenderObject(std::wstring meshPath,
						   float locationX, float locationY, float locationZ,
						   float rotationX, float rotationY, float rotationZ,
						   float scaleX, float scaleY, float scaleZ)
{
	this->meshPath = meshPath;
	MoveTo(locationX, locationY, locationZ);
	RotateTo(rotationX, rotationY, rotationZ);
	ScaleTo(scaleX, scaleY, scaleZ);
}

std::wstring RenderObject::GetMeshPath()
{
	return meshPath;
}

void RenderObject::MoveTo(float locationX, float locationY, float locationZ)
{
	this->locationX = locationX;
	this->locationY = locationY;
	this->locationZ = locationZ;
}

void RenderObject::RotateTo(float rotationX, float rotationY, float rotationZ)
{
	this->rotationX = rotationX;
	this->rotationY = rotationY;
	this->rotationZ = rotationZ;
}

void RenderObject::ScaleTo(float scaleX, float scaleY, float scaleZ)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
	this->scaleZ = scaleZ;
}

std::tuple<float, float, float> RenderObject::GetPosition()
{
	return std::make_tuple(locationX, locationY, locationZ);
}

std::tuple<float, float, float> RenderObject::GetRotation()
{
	return std::make_tuple(rotationX, rotationY, rotationZ);
}

std::tuple<float, float, float> RenderObject::GetScale()
{
	return std::make_tuple(scaleX, scaleY, scaleZ);
}

RenderObject::~RenderObject()
{
}
