#include "Mesh.h"


Mesh::~Mesh()
{
	vertexBuffer->Release();
	indexBuffer->Release();
	if (texture != NULL)
	{
		texture->Release();
	}
}
