#pragma once
#include "CoreD3dService.h"
#include "ConstantBufferTracker.h"
#include <stdexcept>

template <typename BufferStruct> class ConstantBuffer : public ConstantBufferTracker
{
	private:
		ID3D11Buffer* innerBuffer;
		CoreD3dService* coreService;
		unsigned char occupiedSlot;

		void reserveBufferAllocationSlot()
		{
			unsigned char i = 0;
			do
			{
				if (!slotAllocatedFlags[i])
				{
					slotAllocatedFlags[i] = true;
					occupiedSlot = i;
					return;
				}
			} while (++i);
			throw std::length_error("Too many constant buffers allocated.");
		}
	public:
		ConstantBuffer(UINT byteWidth, CoreD3dService* coreService)
		{
			this->coreService = coreService;

			D3D11_BUFFER_DESC buffDesc;
			ZeroMemory(&buffDesc, sizeof(buffDesc));
			buffDesc.Usage = D3D11_USAGE_DEFAULT;
			buffDesc.ByteWidth = (byteWidth / 16 + (byteWidth % 16 > 0 ? 1 : 0)) * 16;
			buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			coreService->CreateBuffer(&buffDesc, &innerBuffer);
			reserveBufferAllocationSlot();
			coreService->RegisterConstantBuffer(occupiedSlot, &innerBuffer);
		}

		void Update(const BufferStruct* updateStruct)
		{
			coreService->UpdateDefaultBuffer(innerBuffer, updateStruct);
		}

		~ConstantBuffer()
		{
			innerBuffer->Release();
			slotAllocatedFlags[occupiedSlot] = false;
		}
};

