#include "MultipleCameraService.h"


MultipleCameraService::MultipleCameraService(CameraProperties firstCamera)
{
	cameraList.push_back(firstCamera);
	activeCameraId = 0;
}

CameraProperties MultipleCameraService::getCameraPropertiesForRendering()
{
	return cameraList[activeCameraId];
}

int MultipleCameraService::addCamera(CameraProperties newCamera)
{
	cameraList.push_back(newCamera);
	return cameraList.size() - 1;
}

void MultipleCameraService::setActiveCamera(int cameraId)
{
	activeCameraId = cameraId;
}

void MultipleCameraService::deleteCamera(int cameraId)
{
	cameraList.erase(cameraList.begin() + cameraId);
}

MultipleCameraService::~MultipleCameraService()
{
}
