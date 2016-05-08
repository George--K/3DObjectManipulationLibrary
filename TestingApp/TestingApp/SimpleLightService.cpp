#include "SimpleLightService.h"


SimpleLightService::SimpleLightService(CoreD3dService* coreService, D3DXVECTOR4 directionalLightVector, D3DXCOLOR directionalLightColor, D3DXCOLOR ambientLightColor)
{
	this->lightState.directionalLightVector = directionalLightVector;
	this->lightState.directionalLightColor = directionalLightColor;
	this->lightState.ambientLightColor = ambientLightColor;

	simpleLightBuffer = new ConstantBuffer<LIGHT_BUFFER>(16 * 3, coreService);
}

void SimpleLightService::UpdateLightBuffers()
{
	if (changed)
	{
		simpleLightBuffer->Update(&lightState);
		changed = false;
	}
}

LIGHT_BUFFER SimpleLightService::getLighting()
{
	return lightState;
}

void SimpleLightService::setLighting(LIGHT_BUFFER lighting)
{
	changed = true;
	lightState = lighting;
}

SimpleLightService::~SimpleLightService()
{
	delete simpleLightBuffer;
}
