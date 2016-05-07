#pragma once
#include "CoreD3dService.h"
#include "ILightService.h"

struct LIGHT_BUFFER
{
	D3DXVECTOR4 directionalLightVector;
	D3DXCOLOR directionalLightColor;
	D3DXCOLOR ambientLightColor;
};

class SimpleLightService : public ILightService
{
	protected:
		CoreD3dService* coreService;
		ID3D11Buffer *lightD3dBuffer;
		LIGHT_BUFFER lightState;
		bool changed = true;

		virtual void initBuffers();
	public:
		SimpleLightService(CoreD3dService* coreService, D3DXVECTOR4 directionalLightVector, D3DXCOLOR directionalLightColor, D3DXCOLOR ambientLightColor);
		virtual void UpdateLightBuffers();
		LIGHT_BUFFER getLighting();
		void setLighting(LIGHT_BUFFER lighting);
		~SimpleLightService();
};

