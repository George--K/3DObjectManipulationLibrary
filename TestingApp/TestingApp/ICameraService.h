#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

struct CameraProperties
{
	D3DXVECTOR3 Location;
	D3DXVECTOR3 LookAt;
	D3DXVECTOR3 UpDirection;
};

class ICameraService
{
	public:
		virtual CameraProperties getCameraPropertiesForRendering() = 0;
};

