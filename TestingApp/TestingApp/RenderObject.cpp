#include "RenderObject.h"


RenderObject::RenderObject(float locationX, float locationY, float locationZ,
						   float rotationX, float rotationY, float rotationZ,
						   float scaleX, float scaleY, float scaleZ)
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

RenderObject::~RenderObject()
{
}
