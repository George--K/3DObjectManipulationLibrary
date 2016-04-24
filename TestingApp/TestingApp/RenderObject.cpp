#include "RenderObject.h"


RenderObject::RenderObject(float locationX = 0.0f, float locationY = 0.0f, float locationZ = 0.0f,
						   float rotationX = 0.0f, float rotationY = 0.0f, float rotationZ = 0.0f,
						   float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f)
{
	this->locationX = locationX;
	this->locationY = locationY;
	this->locationZ = locationZ;
	this->rotationX = rotationX;
	this->rotationY = rotationY;
	this->rotationZ = rotationZ;
	this->scaleX = scaleX;
	this->scaleY = scaleY;
	this->scaleZ = scaleZ;
}

void RenderObject::MoveTo(float locationX, float locationY, float locationZ)
{
	this->locationX = locationX;
	this->locationY = locationY;
	this->locationZ = locationZ;
}

void RenderObject::setRotation(float rotationX, float rotationY, float rotationZ)
{
	this->rotationX = rotationX;
	this->rotationY = rotationY;
	this->rotationZ = rotationZ;
}

void RenderObject::setScaling(float scaleX, float scaleY, float scaleZ)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
	this->scaleZ = scaleZ;
}

RenderObject::~RenderObject()
{
}
