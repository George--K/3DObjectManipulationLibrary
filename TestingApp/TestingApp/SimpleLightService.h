#pragma once
#include "ILightService.h"
#include "CoreD3dService.h"
#include "ConstantBuffer.h"

struct LIGHT_BUFFER
{
	D3DXVECTOR4 directionalLightVector;
	D3DXCOLOR directionalLightColor;
	D3DXCOLOR ambientLightColor;
};

class SimpleLightService : public ILightService
{
	private:
		ConstantBuffer<LIGHT_BUFFER>* simpleLightBuffer;
		LIGHT_BUFFER lightState;
		bool changed = true;
	public:
		SimpleLightService(CoreD3dService* coreService, D3DXVECTOR4 directionalLightVector, D3DXCOLOR directionalLightColor, D3DXCOLOR ambientLightColor);
		virtual void UpdateLightBuffers();
		LIGHT_BUFFER getLighting();
		void setLighting(LIGHT_BUFFER lighting);
		~SimpleLightService();
};

