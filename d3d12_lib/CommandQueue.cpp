#include "pch.h"
#include "CommandQueue.h"
#include "Device.h"

CommandQueue::CommandQueue(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE type)
	: m_AllocatorPool(device, type)
	, m_Device(device)
	, m_Type(type)
	, m_NextFenceValue(((uint64_t)m_Type << 56) + 1)
	, m_LastCompletedFenceValue(((uint64_t)m_Type << 56))
	, m_CommandQueue(nullptr)
	, m_Fence(nullptr)
	, m_FenceHandle(nullptr)
{
	auto d3dDevice = m_Device->GetDevice();
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.NodeMask = 0;
	queueDesc.Type = m_Type;

	d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));

	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
	m_Fence->Signal(m_LastCompletedFenceValue);

	m_FenceHandle = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	ASSERT(m_FenceHandle != INVALID_HANDLE_VALUE);
}

CommandQueue::~CommandQueue()
{
	//CloseHandle(m_fenceHandle);
	//m_Fence->Release();
	//m_CommandQueue->Release();
	//m_CommandQueue = nullptr;
	//m_Fence = nullptr;
	//m_FenceHandle = nullptr;
}

bool CommandQueue::IsFenceCompleted(uint64_t fenceValue)
{
	if (fenceValue > m_LastCompletedFenceValue)
		PollCurrentFenceValue();

	return fenceValue <= m_LastCompletedFenceValue;
}

void CommandQueue::InsertWait(uint64_t fenceValue)
{
	m_CommandQueue->Wait(m_Fence.Get(), fenceValue);
}

void CommandQueue::InsertWaitForOtherQueueFence(CommandQueue* otherQueue, uint64_t fenceValue)
{
	m_CommandQueue->Wait(otherQueue->GetFence().Get(), fenceValue);
}

void CommandQueue::InsertWaitForOtherQueue(CommandQueue* otherQueue)
{
	m_CommandQueue->Wait(otherQueue->GetFence().Get(), otherQueue->GetNextFenceValue());
}

void CommandQueue::WaitForFenceCPUBlocking(uint64_t fenceValue)
{
	if (IsFenceCompleted(fenceValue))
		return;
	{
		std::lock_guard<std::mutex> LockGuard(m_EventMutex);
		m_Fence->SetEventOnCompletion(fenceValue, m_FenceHandle);
		WaitForSingleObjectEx(m_FenceHandle, INFINITE, false);
		m_LastCompletedFenceValue = fenceValue;
	}
}

uint64_t CommandQueue::PollCurrentFenceValue()
{
	m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_Fence->GetCompletedValue());
	return m_LastCompletedFenceValue;
}

uint64_t CommandQueue::ExecuteCommandList(ComPtr<ID3D12CommandList> commandList)
{
	ThrowIfFailed(((ID3D12GraphicsCommandList*)commandList.Get())->Close());
	m_CommandQueue->ExecuteCommandLists(1, commandList.GetAddressOf());
	std::lock_guard<std::mutex> LockGuard(m_FenceMutex);
	m_CommandQueue->Signal(m_Fence.Get(), m_NextFenceValue);

	return m_NextFenceValue++;
}

ComPtr<ID3D12CommandAllocator> CommandQueue::RequestAllocator()
{
	uint64_t completedFence = m_Fence->GetCompletedValue();
	return m_AllocatorPool.RequestAllocator(completedFence);
}

void CommandQueue::DiscardAllocator(uint64_t fenceValue, ComPtr<ID3D12CommandAllocator> allocator)
{
	m_AllocatorPool.DiscardAllocator(fenceValue, allocator);
}
