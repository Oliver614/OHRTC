#pragma once


using Microsoft::WRL::ComPtr;

class Device
{
public:
	Device();
	~Device();

public:

	ComPtr<ID3D12Device8> GetDevice() { return m_Device; }
	ComPtr<IDXGIAdapter4> GetAdapter() { return m_Adapter; }
	std::wstring GetDeviceDescription() const { return m_AdapterDesc.Description; }
	bool GetDXRSupported() const { return m_isDxrSupported; }

private:
	void CreateAdapter(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, bool useWarp = false);
	void CreateDevice();
	void CheckFeatureLevel();
	void IsDirectXRaytracingSupported();

private:
	ComPtr<ID3D12Device8> m_Device;
	ComPtr<IDXGIAdapter4> m_Adapter;
	DXGI_ADAPTER_DESC3 m_AdapterDesc;
	D3D_ROOT_SIGNATURE_VERSION m_HighestRootSignatureVersion;
	bool m_isDxrSupported;

};

