#pragma once

#include "CommandQueue.h"
#include "Device.h"

class CommandQueueManager
{
public:
	CommandQueueManager(std::shared_ptr<Device> device);
	~CommandQueueManager();

public:

	std::shared_ptr<CommandQueue> GetGraphicsQueue() { return m_GraphicsQueue; };
	std::shared_ptr<CommandQueue> GetComputeQueue() { return m_ComputeQueue; };
	std::shared_ptr<CommandQueue> GetCopyQueue() { return m_CopyQueue; };

	std::shared_ptr<CommandQueue> GetQueue(D3D12_COMMAND_LIST_TYPE type);

	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceCPUBlocking(uint64_t fenceValue);
	void WaitForAllIdle();

	void CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList** List, ComPtr<ID3D12CommandAllocator>& Allocator);


private:
	std::shared_ptr<Device> m_Device;
    std::shared_ptr<CommandQueue> m_GraphicsQueue;
	std::shared_ptr<CommandQueue> m_ComputeQueue;
	std::shared_ptr<CommandQueue> m_CopyQueue;
};

