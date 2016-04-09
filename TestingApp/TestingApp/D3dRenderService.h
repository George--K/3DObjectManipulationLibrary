#pragma once
class D3dRenderService
{
	private:
		bool stopped = false;
	public:
		D3dRenderService();
		void RenderLoop();
		~D3dRenderService();
};

