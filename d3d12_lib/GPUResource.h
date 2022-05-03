#pragma once
class GPUResource
{
public:
	GPUResource() :
		m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
		m_UsageState(D3D12_RESOURCE_STATE_COMMON),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{
	}
	GPUResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES CurrentState)
		: m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
		,m_Resource(resource)
		,m_UsageState(CurrentState)
		,m_TransitioningState((D3D12_RESOURCE_STATES)-1)

	{
	}

	~GPUResource() = default;

public:

	ComPtr<ID3D12Resource> GetResource() const { return m_Resource; }
	ID3D12Resource* GetResourceRaw() const { return m_Resource.Get(); }
	ID3D12Resource** GetAddressOf() { return m_Resource.GetAddressOf(); }
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }
	uint32_t GetVersionID() const { return m_VersionID; }

private:

	ComPtr<ID3D12Resource> m_Resource;
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

	// Used to identify when a resource changes so descriptors can be copied etc.
	uint32_t m_VersionID = 0;
};

