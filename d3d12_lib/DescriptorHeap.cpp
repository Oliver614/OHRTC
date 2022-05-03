#include "pch.h"
#include "DescriptorHeap.h"

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(uint32_t Count)
{
    if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
    {
        m_CurrentHeap = ResquestNewHeap(m_Type);
        m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
        m_RemainingFreeHandles = m_NumDescriptorsPerHeap;

        if (m_DescriptorSize == 0)
            m_DescriptorSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(m_Type);

    }

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_CurrentHandle;
    m_CurrentHandle.ptr += Count * m_DescriptorSize;
    m_RemainingFreeHandles -= Count;

    return handle;
}

ComPtr<ID3D12DescriptorHeap> DescriptorAllocator::ResquestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
{

    std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);

    D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
    Desc.Type = type;
    Desc.NumDescriptors = m_NumDescriptorsPerHeap;
    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Desc.NodeMask = 1;

    ComPtr<ID3D12DescriptorHeap> newHeap;
    ASSERT_SUCCEEDED(m_Device->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&newHeap)));
    m_DescriptorHeapPool.emplace_back(newHeap);
    return newHeap;
}

void DescriptorHeap::CreateHeap(const std::wstring& heapName, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxCount)
{
    m_HeapDesc.Type = type;
    m_HeapDesc.NumDescriptors = maxCount;
    m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    m_HeapDesc.NodeMask = 1;

    ASSERT_SUCCEEDED(m_Device->GetDevice()->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(&m_Heap)));

#ifdef DEBUG
    m_Heap->SetName(heapName.c_str());
    LOG_INFO(L"Heap Created, Name: " + heapName);
#endif // DEBUG

    m_DescriptorSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
    m_NumFreeDescriptors = m_HeapDesc.NumDescriptors;
    m_FirstHandle = DescriptorHandle(
        m_Heap->GetCPUDescriptorHandleForHeapStart(),
        m_Heap->GetGPUDescriptorHandleForHeapStart());
    m_NextFreeHandle = m_FirstHandle;

}

DescriptorHandle DescriptorHeap::Alloc(uint32_t Count)
{
    ASSERT(HasAvailableSpace(Count), "Descriptor Heap out of space.  Increase heap size.");
    DescriptorHandle ret = m_NextFreeHandle;
    m_NextFreeHandle += Count * m_DescriptorSize;
    m_NumFreeDescriptors -= Count;
    return ret;
}

bool DescriptorHeap::ValidateHandle(const DescriptorHandle& DHandle) const
{
    if (DHandle.GetCpuPtr() < m_FirstHandle.GetCpuPtr() ||
        DHandle.GetCpuPtr() >= m_FirstHandle.GetCpuPtr() + m_HeapDesc.NumDescriptors * m_DescriptorSize)
        return false;

    if (DHandle.GetGpuPtr() - m_FirstHandle.GetGpuPtr() !=
        DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr())
        return false;

    return true;
}
