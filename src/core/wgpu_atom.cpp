//
// Created by Raul Romero on 2024-01-16.
//

#include "wgpu_atom.h"
#include <webgpu/webgpu_cpp.h>
#include <cassert>
#include "Logger.h"

namespace wgpu
{

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

std::string FeatureNameToString(FeatureName featureName)
{
    switch (featureName)
    {
        case FeatureName::Undefined:
            return "Undefined";
        case FeatureName::DepthClipControl:
            return "DepthClipControl";
        case FeatureName::Depth32FloatStencil8:
            return "Depth32FloatStencil8";
        case FeatureName::TimestampQuery:
            return "TimestampQuery";
        case FeatureName::TextureCompressionBC:
            return "TextureCompressionBC";
        case FeatureName::TextureCompressionETC2:
            return "TextureCompressionETC2";
        case FeatureName::TextureCompressionASTC:
            return "TextureCompressionASTC";
        case FeatureName::IndirectFirstInstance:
            return "IndirectFirstInstance";
        case FeatureName::ShaderF16:
            return "ShaderF16";
        case FeatureName::RG11B10UfloatRenderable:
            return "RG11B10UfloatRenderable";
        case FeatureName::BGRA8UnormStorage:
            return "BGRA8UnormStorage";
        case FeatureName::Float32Filterable:
            return "Float32Filterable";
        case FeatureName::DawnInternalUsages:
            return "DawnInternalUsages";
        case FeatureName::DawnMultiPlanarFormats:
            return "DawnMultiPlanarFormats";
        case FeatureName::DawnNative:
            return "DawnNative";
        case FeatureName::ChromiumExperimentalTimestampQueryInsidePasses:
            return "ChromiumExperimentalTimestampQueryInsidePasses";
        case FeatureName::ImplicitDeviceSynchronization:
            return "ImplicitDeviceSynchronization";
        case FeatureName::SurfaceCapabilities:
            return "SurfaceCapabilities";
        case FeatureName::TransientAttachments:
            return "TransientAttachments";
        case FeatureName::MSAARenderToSingleSampled:
            return "MSAARenderToSingleSampled";
        case FeatureName::DualSourceBlending:
            return "DualSourceBlending";
        case FeatureName::D3D11MultithreadProtected:
            return "D3D11MultithreadProtected";
        case FeatureName::ANGLETextureSharing:
            return "ANGLETextureSharing";
        case FeatureName::ChromiumExperimentalSubgroups:
            return "ChromiumExperimentalSubgroups";
        case FeatureName::ChromiumExperimentalSubgroupUniformControlFlow:
            return "ChromiumExperimentalSubgroupUniformControlFlow";
        case FeatureName::PixelLocalStorageCoherent:
            return "PixelLocalStorageCoherent";
        case FeatureName::PixelLocalStorageNonCoherent:
            return "PixelLocalStorageNonCoherent";
        case FeatureName::Norm16TextureFormats:
            return "Norm16TextureFormats";
        case FeatureName::MultiPlanarFormatExtendedUsages:
            return "MultiPlanarFormatExtendedUsages";
        case FeatureName::MultiPlanarFormatP010:
            return "MultiPlanarFormatP010";
        case FeatureName::HostMappedPointer:
            return "HostMappedPointer";
        case FeatureName::MultiPlanarRenderTargets:
            return "MultiPlanarRenderTargets";
        case FeatureName::MultiPlanarFormatNv12a:
            return "MultiPlanarFormatNv12a";
        case FeatureName::FramebufferFetch:
            return "FramebufferFetch";
        case FeatureName::BufferMapExtendedUsages:
            return "BufferMapExtendedUsages";
        case FeatureName::AdapterPropertiesMemoryHeaps:
            return "AdapterPropertiesMemoryHeaps";
        case FeatureName::SharedTextureMemoryVkDedicatedAllocation:
            return "SharedTextureMemoryVkDedicatedAllocation";
        case FeatureName::SharedTextureMemoryAHardwareBuffer:
            return "SharedTextureMemoryAHardwareBuffer";
        case FeatureName::SharedTextureMemoryDmaBuf:
            return "SharedTextureMemoryDmaBuf";
        case FeatureName::SharedTextureMemoryOpaqueFD:
            return "SharedTextureMemoryOpaqueFD";
        case FeatureName::SharedTextureMemoryZirconHandle:
            return "SharedTextureMemoryZirconHandle";
        case FeatureName::SharedTextureMemoryDXGISharedHandle:
            return "SharedTextureMemoryDXGISharedHandle";
        case FeatureName::SharedTextureMemoryD3D11Texture2D:
            return "SharedTextureMemoryD3D11Texture2D";
        case FeatureName::SharedTextureMemoryIOSurface:
            return "SharedTextureMemoryIOSurface";
        case FeatureName::SharedTextureMemoryEGLImage:
            return "SharedTextureMemoryEGLImage";
        case FeatureName::SharedFenceVkSemaphoreOpaqueFD:
            return "SharedFenceVkSemaphoreOpaqueFD";
        case FeatureName::SharedFenceVkSemaphoreSyncFD:
            return "SharedFenceVkSemaphoreSyncFD";
        case FeatureName::SharedFenceVkSemaphoreZirconHandle:
            return "SharedFenceVkSemaphoreZirconHandle";
        case FeatureName::SharedFenceDXGISharedHandle:
            return "SharedFenceDXGISharedHandle";
        case FeatureName::SharedFenceMTLSharedEvent:
            return "SharedFenceMTLSharedEvent";
        default:
            return "Unknown";
    }
}

std::string ErrorTypeToString(ErrorType errorType)
{
    switch(errorType)
    {
        case ErrorType::NoError:
            return "NoError";
        case ErrorType::Validation:
            return "Validation";
        case ErrorType::OutOfMemory:
            return "OutOfMemory";
        case ErrorType::Internal:
            return "Internal";
        case ErrorType::Unknown:
            return "Unknown";
        case ErrorType::DeviceLost:
            return "DeviceLost";
        default:
            return "Unknown";
    }
}

std::string ErrorTypeToString(i32 errorType)
{
    return ErrorTypeToString(static_cast<ErrorType>(errorType));
}

Adapter RequestAdapter(const Instance& instance, const RequestAdapterOptions* options)
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
            ::atom::LogError("Could not get WebGPU adapter: ", message);
        }
        userData.requestEnded = true;
    };

    instance.RequestAdapter(options, onAdapterRequestEnded, (void*)&userData);
    assert(userData.requestEnded);

    return userData.adapter;
}

Device RequestDevice(Adapter adapter, const DeviceDescriptor* descriptor) {
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
            ::atom::LogError("Could not get WebGPU device: %s", message);
        }
        userData.requestEnded = true;
    };

    adapter.RequestDevice(descriptor, onDeviceRequestEnded, (void*)&userData);

    assert(userData.requestEnded);

    return userData.device;
}

} // namespace atom

} // namespace wgpu


