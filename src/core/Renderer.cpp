//
// Created by Raul Romero on 2024-01-13.
//

#include "Renderer.h"
#include <webgpu/webgpu_glfw.h>
#include "Logger.h"
#include "Engine.h"
#include <iostream>
#include "wgpu_atom.h"


namespace atom
{

Renderer::Renderer() : m_Instance(nullptr), m_Surface(nullptr), m_Adapter(nullptr), m_Device(nullptr)
{

}

Renderer::~Renderer() = default;

void Renderer::Init()
{
    SetupInstance();
    SetupSurface();
    SetupAdapter();
    SetupDevice();
    SetupQueue();
    SetupSwapChain();
    SetupRenderPipeline();
}

void Renderer::Draw()
{
    wgpu::TextureView backBuffer = m_SwapChain.GetCurrentTextureView();
    wgpu::CommandEncoder encoder = m_Device.CreateCommandEncoder();
    wgpu::RenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.label = "My render pass";
    wgpu::RenderPassColorAttachment colorAttachment = {};
    colorAttachment.view = backBuffer;
    colorAttachment.resolveTarget = nullptr;
    colorAttachment.clearValue = {0.0f, 0.0f, 0.0f, 1.0f};
    colorAttachment.loadOp = wgpu::LoadOp::Clear;
    colorAttachment.storeOp = wgpu::StoreOp::Store;
    renderPassDesc.timestampWrites = nullptr;
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);
    renderPass.SetPipeline(m_RenderPipeline);
    renderPass.Draw(3, 1, 0, 0);
    renderPass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    m_Queue.Submit(1, &commands);
    m_SwapChain.Present();
}

Renderer *Renderer::Create()
{
    Renderer* pRenderer = new Renderer();
    pRenderer->Init();
    return pRenderer;
}

void Renderer::SetupInstance()
{
    wgpu::InstanceDescriptor instanceDesc = {};
    instanceDesc.nextInChain = nullptr;

    m_Instance = wgpu::CreateInstance(&instanceDesc);

    if(m_Instance == nullptr)
    {
        LogError("Failed to create WebGPU instance.");
        return;
    }
}

void Renderer::SetupSurface()
{
    m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, Engine::Instance().GetGLFWWindow());
}

void Renderer::SetupAdapter()
{
    wgpu::RequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = m_Surface;
    adapterOpts.backendType = wgpu::BackendType::D3D12;

    m_Adapter = wgpu::atom::RequestAdapter(m_Instance, &adapterOpts);

    std::vector<wgpu::FeatureName> features;
    size_t featureCount = m_Adapter.EnumerateFeatures(nullptr);
    features.resize(featureCount);
    m_Adapter.EnumerateFeatures(features.data());

    for (wgpu::FeatureName f : features)
    {
        LogInfo("Adapter feature [%s] is enabled", wgpu::atom::FeatureNameToString(f).c_str());
    }
}

void Renderer::SetupDevice()
{
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.label = "My Device";
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "The default queue";
    m_Device = wgpu::atom::RequestDevice(m_Adapter, &deviceDesc);
    LogInfo("Device created: %p", m_Device.Get());

    auto onDeviceError = [](WGPUErrorType type, const char* message, [[maybe_unused]] void* userdata)
    {
        const char *errorTypeString;
        errorTypeString = wgpu::atom::ErrorTypeToString(type).c_str();
        LogError("Device %s: %s", errorTypeString, message);
    };
    m_Device.SetUncapturedErrorCallback(onDeviceError, nullptr);
}

void Renderer::SetupQueue()
{
    m_Queue = m_Device.GetQueue();

    auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, [[maybe_unused]] void* userdata)
    {
        LogInfo("Queue work done: %d", status);
    };
    m_Queue.OnSubmittedWorkDone(onQueueWorkDone, nullptr);
}

void Renderer::SetupSwapChain()
{
    wgpu::SwapChainDescriptor swapChainDesc = {};
    swapChainDesc.nextInChain = nullptr;
    swapChainDesc.label = "My Swap Chain";
    swapChainDesc.width = 640;
    swapChainDesc.height = 480;
    swapChainDesc.format = wgpu::TextureFormat::BGRA8Unorm;
    swapChainDesc.usage = wgpu::TextureUsage::RenderAttachment;
    swapChainDesc.presentMode = wgpu::PresentMode::Fifo;
    m_SwapChain = m_Device.CreateSwapChain(m_Surface, &swapChainDesc);
}

void Renderer::SetupRenderPipeline()
{
    const char* shaderSource = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
            var p = vec2f(0.0, 0.0);
            if (in_vertex_index == 0u) {
                p = vec2f(-0.5, -0.5);
            } else if (in_vertex_index == 1u) {
                p = vec2f(0.5, -0.5);
            } else {
                p = vec2f(0.0, 0.5);
            }
            return vec4f(p, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.0, 0.4, 1.0, 1.0);
        }
    )";

    wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
    shaderCodeDesc.code = shaderSource;

    wgpu::ShaderModuleDescriptor shaderDesc = {};
    #ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;
    #endif
    shaderDesc.nextInChain = &shaderCodeDesc;
    wgpu::ShaderModule shaderModule = m_Device.CreateShaderModule(&shaderDesc);

    wgpu::RenderPipelineDescriptor pipelineDesc = {};

    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    wgpu::FragmentState fragmentState;
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.depthStencil = nullptr;

    wgpu::BlendState blendState;

    wgpu::ColorTargetState colorTarget;
    colorTarget.format = m_SwapChain.GetCurrentTexture().GetFormat();
    colorTarget.blend = &blendState;
    colorTarget.writeMask = wgpu::ColorWriteMask::All; // We could write to only some of the color channels.

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    pipelineDesc.layout = nullptr;

    m_RenderPipeline = m_Device.CreateRenderPipeline(&pipelineDesc);
}

}