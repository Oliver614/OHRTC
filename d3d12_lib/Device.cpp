#include "pch.h"
#include "Device.h"

Device::Device()
{
    CreateAdapter();
    CreateDevice();
    CheckFeatureLevel();
}

Device::~Device()
{
}

void Device::CreateAdapter(DXGI_GPU_PREFERENCE gpuPreference, bool useWarp)
{
    ComPtr<IDXGIFactory6> dxgiFactory6;
    ComPtr<IDXGIAdapter>  dxgiAdapter;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    UINT createFactoryFlags = 0;

#if defined( _DEBUG )
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));

    if (useWarp)
    {
        ThrowIfFailed(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
        ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
    }
    else
    {
        for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
            DXGI_ERROR_NOT_FOUND;
            ++i)
        {
            if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device),
                nullptr)))
            {
                ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
                break;
            }
        }
    }

    if (dxgiAdapter4)
    {
        m_Adapter = dxgiAdapter4;
        m_Adapter->GetDesc3(&m_AdapterDesc);

        LOG_INFO(L"Adapter Created: {}", m_AdapterDesc.Description);
    }
}

void Device::CreateDevice()
{
    ASSERT(m_Adapter);
    ThrowIfFailed(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));

    // Enable debug messages (only works if the debug layer has already been enabled).
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(m_Device.As(&pInfoQueue)))
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress whole categories of messages
        // D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,  // I'm really not sure how to avoid this
                                                                           // message.

            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,  // This warning occurs when using capture frame while graphics
                                                     // debugging.

            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,  // This warning occurs when using capture frame while graphics
                                                       // debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        // NewFilter.DenyList.NumCategories = _countof(Categories);
        // NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
    }
}

void Device::CheckFeatureLevel()
{
    ASSERT(m_Device);
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData;
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
        sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    m_HighestRootSignatureVersion = featureData.HighestVersion;
    LOG_INFO("Highest Root Sig Version: {}", m_HighestRootSignatureVersion);
}

void Device::IsDirectXRaytracingSupported()
{
    ASSERT(m_Adapter);
    ComPtr<ID3D12Device> testDevice;
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

    m_isDxrSupported = SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
        && SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
        && featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}
