#include "pch.h"
#include "CommandAllocatorPool.h"
#include "Device.h"

CommandAllocatorPool::CommandAllocatorPool(std::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE type)
	: m_Device(device)
	, m_Type(type)
{
}

CommandAllocatorPool::~CommandAllocatorPool()
{
	for (size_t i = 0; i < m_AllocatorPool.size(); i++)
	{
		m_AllocatorPool[i]->Release();
	}
	m_AllocatorPool.clear();
}

ComPtr<ID3D12CommandAllocator> CommandAllocatorPool::RequestAllocator(uint64_t completedFenceValue)
{
	std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);
	auto device = m_Device->GetDevice();
	ComPtr<ID3D12CommandAllocator> newAllocator = nullptr;

	if (!m_ReadyAllocators.empty())
	{
		std::pair<uint64_t, ComPtr<ID3D12CommandAllocator>>& allocatorPair = m_ReadyAllocators.front();
		if (allocatorPair.first <= completedFenceValue)
		{
			newAllocator = allocatorPair.second;
			ASSERT_SUCCEEDED(newAllocator->Reset());
			m_ReadyAllocators.pop();

		}
	}

	if (newAllocator == nullptr)
	{
		ASSERT_SUCCEEDED(device->CreateCommandAllocator(m_Type, IID_PPV_ARGS(&newAllocator)));
		wchar_t allocName[32];
		swprintf(allocName, 32, L"CommandAllocator %zu", m_AllocatorPool.size());
		newAllocator->SetName(allocName);
		m_AllocatorPool.push_back(newAllocator);
	}

	return newAllocator;
}

void CommandAllocatorPool::DiscardAllocator(uint64_t fenceValue, ComPtr<ID3D12CommandAllocator> allocator)
{
	std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);
	m_ReadyAllocators.push(std::make_pair(fenceValue, allocator));
}
