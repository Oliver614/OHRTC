#pragma once

#include "CommandAllocatorPool.h"

using Microsoft::WRL::ComPtr;

class Device;

class CommandQueue
{
public:
	CommandQueue(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE type);
	~CommandQueue();

public:
	
	bool IsFenceCompleted(uint64_t fenceValue);
	void InsertWait(uint64_t fenceValue);
	void InsertWaitForOtherQueueFence(CommandQueue* otherQueue, uint64_t fenceValue);
	void InsertWaitForOtherQueue(CommandQueue* otherQueue);

	void WaitForFenceCPUBlocking(uint64_t fenceValue);
	void WaitForIdle() { WaitForFenceCPUBlocking(m_NextFenceValue - 1); }

	ComPtr<ID3D12CommandQueue> GetQueue() { return m_CommandQueue; }

	uint64_t PollCurrentFenceValue();
	uint64_t GetLastCompletedFence() { return m_LastCompletedFenceValue; }
	uint64_t GetNextFenceValue() { return m_NextFenceValue; }
	ComPtr<ID3D12Fence> GetFence() { return m_Fence; }

public:

	uint64_t ExecuteCommandList(ComPtr<ID3D12CommandList> commandList);
	ComPtr<ID3D12CommandAllocator> RequestAllocator();
	void DiscardAllocator(uint64_t fenceValue, ComPtr<ID3D12CommandAllocator> allocator);

private:

	std::shared_ptr<Device> m_Device;
	D3D12_COMMAND_LIST_TYPE m_Type;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12Fence> m_Fence;
	CommandAllocatorPool m_AllocatorPool;

	uint64_t m_NextFenceValue;
	uint64_t m_LastCompletedFenceValue;
	HANDLE m_FenceHandle;

	std::mutex m_EventMutex;
	std::mutex m_FenceMutex;
};

