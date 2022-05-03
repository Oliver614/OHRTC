#include "pch.h"
#include "Context.h"

Context::Context()
{
	m_Device = std::make_shared<Device>();
	m_CommandQueueManager = std::make_shared<CommandQueueManager>(m_Device);
	
}

void Context::Test()
{
	ComPtr<ID3D12GraphicsCommandList> list;
	ComPtr<ID3D12CommandAllocator> allocator;

	m_CommandQueueManager->CreateNewCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, list.GetAddressOf(), allocator);

	ASSERT(list);
	ASSERT(allocator);
	list->SetName(L"Test Name");
	

	uint64_t number = m_CommandQueueManager->GetGraphicsQueue()->ExecuteCommandList(list);
}
