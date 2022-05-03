#pragma once

#include <queue>

class Device;

class CommandAllocatorPool
{
public: 
	CommandAllocatorPool(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE type);
	~CommandAllocatorPool();

public:

	ComPtr<ID3D12CommandAllocator> RequestAllocator(uint64_t completedFenceValue);
	void DiscardAllocator(uint64_t fenceValue, ComPtr<ID3D12CommandAllocator> allocator);
	size_t Size() { return m_AllocatorPool.size(); }

public:

	std::shared_ptr<Device> m_Device;
	D3D12_COMMAND_LIST_TYPE m_Type;
	std::vector<ComPtr<ID3D12CommandAllocator>> m_AllocatorPool;
	std::queue<std::pair<uint64_t, ComPtr<ID3D12CommandAllocator>>> m_ReadyAllocators;
	std::mutex m_AllocatorMutex;


};

