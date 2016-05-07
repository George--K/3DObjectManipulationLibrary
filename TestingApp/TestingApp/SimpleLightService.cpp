#include "SimpleLightService.h"


SimpleLightService::SimpleLightService(CoreD3dService* coreService, D3DXVECTOR4 directionalLightVector, D3DXCOLOR directionalLightColor, D3DXCOLOR ambientLightColor)
{
	this->coreService = coreService;
	this->lightState.directionalLightVector = directionalLightVector;
	this->lightState.directionalLightColor = directionalLightColor;
	this->lightState.ambientLightColor = ambientLightColor;
	initBuffers();
}

void SimpleLightService::initBuffers()
{
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.ByteWidth = sizeof(LIGHT_BUFFER);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	coreService->getDeviceInterface()->CreateBuffer(&buffDesc, NULL, &lightD3dBuffer);
	coreService->getDeviceContext()->VSSetConstantBuffers(0, 1, &lightD3dBuffer);
}

void SimpleLightService::UpdateLightBuffers()
{
	if (changed)
	{
		coreService->getDeviceContext()->UpdateSubresource(lightD3dBuffer, NULL, NULL, &lightState, 0, 0);
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
	lightD3dBuffer->Release();
}
