#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <Windows.h>
#include <shellapi.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <dxgi1_6.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

using namespace DirectX;
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

// STL Headers
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "HelperFuncs.h"
#include "Log.h"



