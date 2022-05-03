#pragma once

#include <mutex>
#include <vector>
#include <queue>
#include <string>

#include "Device.h"

class DescriptorAllocator
{
public: 
	DescriptorAllocator(std::shared_ptr<Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type) :
		m_Device(device), m_Type(type), m_CurrentHeap(nullptr), m_DescriptorSize(0)
	{
		m_CurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t Count);

	~DescriptorAllocator() = default;

private:

	std::shared_ptr<Device> m_Device;

	static const uint32_t m_NumDescriptorsPerHeap = 256;
	std::mutex m_AllocationMutex;
	std::vector<ComPtr<ID3D12DescriptorHeap>> m_DescriptorHeapPool;
	ComPtr<ID3D12DescriptorHeap> ResquestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

	D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
	ComPtr<ID3D12DescriptorHeap> m_CurrentHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;
	uint32_t m_DescriptorSize;
	uint32_t m_RemainingFreeHandles;

};

class DescriptorHandle
{
public:
	DescriptorHandle()
	{
		m_CpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_CpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		: m_CpuHandle(cpuHandle), m_GpuHandle(gpuHandle)
	{

	}

	DescriptorHandle operator+ (INT OffsetScaledByDescriptorSize) const
	{
		DescriptorHandle ret = *this;
		ret += OffsetScaledByDescriptorSize;
		return ret;
	}

	void operator += (INT OffsetScaledByDescriptorSize)
	{
		if (m_CpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
			m_CpuHandle.ptr += OffsetScaledByDescriptorSize;
		if (m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
			m_GpuHandle.ptr += OffsetScaledByDescriptorSize;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &m_CpuHandle; }
	operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }
	operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }

	size_t GetCpuPtr() const { return m_CpuHandle.ptr; }
	uint64_t GetGpuPtr() const { return m_GpuHandle.ptr; }
	bool IsNull() const { return m_CpuHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
	bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }

private:

	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;

};

class DescriptorHeap
{
	DescriptorHeap(std::shared_ptr<Device> device) :
		m_Device(device)
	{

	}
	~DescriptorHeap() = default;

	void CreateHeap(const std::wstring& heapName, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxCount);

	bool HasAvailableSpace(uint32_t Count) const { return Count <= m_NumFreeDescriptors; }
	DescriptorHandle Alloc(uint32_t Count = 1);

	DescriptorHandle operator[] (uint32_t arrayIdx) const { return m_FirstHandle + arrayIdx * m_DescriptorSize; }

	uint32_t GetOffsetOfHandle(const DescriptorHandle& DHandle) {
		return (uint32_t)(DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr()) / m_DescriptorSize;
	}

	bool ValidateHandle(const DescriptorHandle& DHandle) const;

	ID3D12DescriptorHeap* GetHeapPointer() const { return m_Heap.Get(); }

	uint32_t GetDescriptorSize(void) const { return m_DescriptorSize; }

private:

	std::shared_ptr<Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
	D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
	uint32_t m_DescriptorSize;
	uint32_t m_NumFreeDescriptors;
	DescriptorHandle m_FirstHandle;
	DescriptorHandle m_NextFreeHandle;
};

