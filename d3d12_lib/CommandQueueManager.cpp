#include "pch.h"
#include "CommandQueueManager.h"

CommandQueueManager::CommandQueueManager(std::shared_ptr<Device> device)
	: m_Device(device)
{
	m_GraphicsQueue = std::make_shared<CommandQueue>(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_ComputeQueue = std::make_shared<CommandQueue>(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	m_CopyQueue = std::make_shared<CommandQueue>(device, D3D12_COMMAND_LIST_TYPE_COPY);
}

CommandQueueManager::~CommandQueueManager()
{
}

std::shared_ptr<CommandQueue> CommandQueueManager::GetQueue(D3D12_COMMAND_LIST_TYPE type)
{

	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		return m_GraphicsQueue;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		return m_ComputeQueue;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return m_CopyQueue;
	}

	return nullptr;

}

bool CommandQueueManager::IsFenceComplete(uint64_t fenceValue)
{
	return GetQueue((D3D12_COMMAND_LIST_TYPE)(fenceValue >> 56))->IsFenceCompleted(fenceValue);
}

void CommandQueueManager::WaitForFenceCPUBlocking(uint64_t fenceValue)
{
	std::shared_ptr<CommandQueue> commandQueue = GetQueue((D3D12_COMMAND_LIST_TYPE)(fenceValue >> 56));
	commandQueue->WaitForFenceCPUBlocking(fenceValue);
}

void CommandQueueManager::WaitForAllIdle()
{
	m_GraphicsQueue->WaitForIdle();
	m_ComputeQueue->WaitForIdle();
	m_CopyQueue->WaitForIdle();
}

void CommandQueueManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList** List, ComPtr<ID3D12CommandAllocator>& Allocator)
{
	ASSERT(Type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Bundles are not yet supported");
	switch (Type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT: Allocator = m_GraphicsQueue->RequestAllocator(); break;
	case D3D12_COMMAND_LIST_TYPE_BUNDLE: break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE: Allocator = m_ComputeQueue->RequestAllocator(); break;
	case D3D12_COMMAND_LIST_TYPE_COPY: Allocator = m_CopyQueue->RequestAllocator(); break;
	}
	auto device = m_Device->GetDevice();
	ASSERT_SUCCEEDED(device->CreateCommandList(1, Type, Allocator.Get(), nullptr, IID_PPV_ARGS(List)));
	(*List)->SetName(L"CommandList");
}
