#pragma once
#include <vector>
#include "ICameraService.h"
class MultipleCameraService : public ICameraService
{
	private:
		std::vector<CameraProperties> cameraList;
		int activeCameraId;
	public:
		MultipleCameraService(CameraProperties firstCamera);
		virtual CameraProperties getCameraPropertiesForRendering();
		int addCamera(CameraProperties newCamera);
		void setActiveCamera(int cameraId);
		void deleteCamera(int cameraId);
		~MultipleCameraService();
};

