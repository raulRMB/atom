//
// Created by Raul Romero on 2024-01-16.
//

#include "wgpu_atom.h"
#include <cassert>
#include "Logger.h"

#include <webgpu/webgpu_cpp.h>

#if defined(DAWN_ENABLE_D3D12)
    static wgpu::BackendType backendType = wgpu::BackendType::D3D12;
#elif defined(DAWN_ENABLE_D3D11)
    static wgpu::BackendType backendType = wgpu::BackendType::D3D11;
#elif defined(DAWN_ENABLE_METAL)
    static wgpu::BackendType backendType = wgpu::BackendType::Metal;
#elif defined(DAWN_ENABLE_VULKAN)
    static wgpu::BackendType backendType = wgpu::BackendType::Vulkan;
#elif defined(DAWN_ENABLE_OPENGLES)
    static wgpu::BackendType backendType = wgpu::BackendType::OpenGLES;
#elif defined(DAWN_ENABLE_DESKTOP_GL)
    static wgpu::BackendType backendType = wgpu::BackendType::OpenGL;
#endif

namespace atom
{

std::string FeatureNameToString([[maybe_unused]] wgpu::FeatureName featureName)
{
#if !defined(__EMSCRIPTEN__)
    switch (featureName)
    {
        case wgpu::FeatureName::Undefined:
            return "Undefined";
        case wgpu::FeatureName::DepthClipControl:
            return "DepthClipControl";
        case wgpu::FeatureName::Depth32FloatStencil8:
            return "Depth32FloatStencil8";
        case wgpu::FeatureName::TimestampQuery:
            return "TimestampQuery";
        case wgpu::FeatureName::TextureCompressionBC:
            return "TextureCompressionBC";
        case wgpu::FeatureName::TextureCompressionETC2:
            return "TextureCompressionETC2";
        case wgpu::FeatureName::TextureCompressionASTC:
            return "TextureCompressionASTC";
        case wgpu::FeatureName::IndirectFirstInstance:
            return "IndirectFirstInstance";
        case wgpu::FeatureName::ShaderF16:
            return "ShaderF16";
        case wgpu::FeatureName::RG11B10UfloatRenderable:
            return "RG11B10UfloatRenderable";
        case wgpu::FeatureName::BGRA8UnormStorage:
            return "BGRA8UnormStorage";
        case wgpu::FeatureName::Float32Filterable:
            return "Float32Filterable";
        case wgpu::FeatureName::DawnInternalUsages:
            return "DawnInternalUsages";
        case wgpu::FeatureName::DawnMultiPlanarFormats:
            return "DawnMultiPlanarFormats";
        case wgpu::FeatureName::DawnNative:
            return "DawnNative";
        case wgpu::FeatureName::ChromiumExperimentalTimestampQueryInsidePasses:
            return "ChromiumExperimentalTimestampQueryInsidePasses";
        case wgpu::FeatureName::ImplicitDeviceSynchronization:
            return "ImplicitDeviceSynchronization";
        case wgpu::FeatureName::SurfaceCapabilities:
            return "SurfaceCapabilities";
        case wgpu::FeatureName::TransientAttachments:
            return "TransientAttachments";
        case wgpu::FeatureName::MSAARenderToSingleSampled:
            return "MSAARenderToSingleSampled";
        case wgpu::FeatureName::DualSourceBlending:
            return "DualSourceBlending";
        case wgpu::FeatureName::D3D11MultithreadProtected:
            return "D3D11MultithreadProtected";
        case wgpu::FeatureName::ANGLETextureSharing:
            return "ANGLETextureSharing";
        case wgpu::FeatureName::ChromiumExperimentalSubgroups:
            return "ChromiumExperimentalSubgroups";
        case wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow:
            return "ChromiumExperimentalSubgroupUniformControlFlow";
        case wgpu::FeatureName::PixelLocalStorageCoherent:
            return "PixelLocalStorageCoherent";
        case wgpu::FeatureName::PixelLocalStorageNonCoherent:
            return "PixelLocalStorageNonCoherent";
        case wgpu::FeatureName::Norm16TextureFormats:
            return "Norm16TextureFormats";
        case wgpu::FeatureName::MultiPlanarFormatExtendedUsages:
            return "MultiPlanarFormatExtendedUsages";
        case wgpu::FeatureName::MultiPlanarFormatP010:
            return "MultiPlanarFormatP010";
        case wgpu::FeatureName::HostMappedPointer:
            return "HostMappedPointer";
        case wgpu::FeatureName::MultiPlanarRenderTargets:
            return "MultiPlanarRenderTargets";
        case wgpu::FeatureName::MultiPlanarFormatNv12a:
            return "MultiPlanarFormatNv12a";
        case wgpu::FeatureName::FramebufferFetch:
            return "FramebufferFetch";
        case wgpu::FeatureName::BufferMapExtendedUsages:
            return "BufferMapExtendedUsages";
        case wgpu::FeatureName::AdapterPropertiesMemoryHeaps:
            return "AdapterPropertiesMemoryHeaps";
        case wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation:
            return "SharedTextureMemoryVkDedicatedAllocation";
        case wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer:
            return "SharedTextureMemoryAHardwareBuffer";
        case wgpu::FeatureName::SharedTextureMemoryDmaBuf:
            return "SharedTextureMemoryDmaBuf";
        case wgpu::FeatureName::SharedTextureMemoryOpaqueFD:
            return "SharedTextureMemoryOpaqueFD";
        case wgpu::FeatureName::SharedTextureMemoryZirconHandle:
            return "SharedTextureMemoryZirconHandle";
        case wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle:
            return "SharedTextureMemoryDXGISharedHandle";
        case wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D:
            return "SharedTextureMemoryD3D11Texture2D";
        case wgpu::FeatureName::SharedTextureMemoryIOSurface:
            return "SharedTextureMemoryIOSurface";
        case wgpu::FeatureName::SharedTextureMemoryEGLImage:
            return "SharedTextureMemoryEGLImage";
        case wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD:
            return "SharedFenceVkSemaphoreOpaqueFD";
        case wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD:
            return "SharedFenceVkSemaphoreSyncFD";
        case wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle:
            return "SharedFenceVkSemaphoreZirconHandle";
        case wgpu::FeatureName::SharedFenceDXGISharedHandle:
            return "SharedFenceDXGISharedHandle";
        case wgpu::FeatureName::SharedFenceMTLSharedEvent:
            return "SharedFenceMTLSharedEvent";
        default:
            return "Unknown";
    }
#endif
    return "Unknown";
}

std::string ErrorTypeToString(wgpu::ErrorType errorType)
{
    switch(errorType)
    {
        case wgpu::ErrorType::NoError:
            return "NoError";
        case wgpu::ErrorType::Validation:
            return "Validation";
        case wgpu::ErrorType::OutOfMemory:
            return "OutOfMemory";
        case wgpu::ErrorType::Internal:
            return "Internal";
        case wgpu::ErrorType::Unknown:
            return "Unknown";
        case wgpu::ErrorType::DeviceLost:
            return "DeviceLost";
        default:
            return "Unknown";
    }
}

std::string ErrorTypeToString(i32 errorType)
{
    return ErrorTypeToString(static_cast<wgpu::ErrorType>(errorType));
}

wgpu::Adapter RequestAdapter(const wgpu::Instance& instance, const wgpu::RequestAdapterOptions* options)
{
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    } userData;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            LogError("Could not get WebGPU adapter: ", message);
        }
        userData.requestEnded = true;
    };

    instance.RequestAdapter(options, onAdapterRequestEnded, (void*)&userData);
    assert(userData.requestEnded);

    return userData.adapter;
}

    wgpu::Device RequestDevice([[maybe_unused]] wgpu::Adapter adapter, [[maybe_unused]] const wgpu::DeviceDescriptor* descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            LogError("Could not get WebGPU device: %s", message);
        }
        userData.requestEnded = true;
    };
    adapter.RequestDevice(descriptor, onDeviceRequestEnded, (void*)&userData);

    assert(userData.requestEnded);

    return userData.device;
}

} // namespace atom


