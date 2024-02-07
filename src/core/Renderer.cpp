//
// Created by Raul Romero on 2024-01-13.
//

#include "Renderer.h"
#include <webgpu/webgpu_glfw.h>
#include "Logger.h"
#include "Engine.h"
#include <iostream>
#include "wgpu_atom.h"
#include "Reader.h"
#include "3dRenderSystem.h"

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
    SetupBuffers();
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

    const Render3dSystem& rend = System::Get<Render3dSystem>();
    rend.OnFrame(renderPass);

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

    wgpu::SupportedLimits supportedLimits;
    m_Adapter.GetLimits(&supportedLimits);

    wgpu::RequiredLimits requiredLimits = {};
    requiredLimits.limits.maxVertexAttributes = 2;
    requiredLimits.limits.maxVertexBuffers = 1;
    requiredLimits.limits.maxBufferSize = 6 * 6 * sizeof(float);
    requiredLimits.limits.maxVertexBufferArrayStride = 6 * sizeof(float);
    requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
    requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
    requiredLimits.limits.maxInterStageShaderComponents = 3;

    deviceDesc.requiredLimits = &requiredLimits;
    m_Device = wgpu::atom::RequestDevice(m_Adapter, &deviceDesc);
    LogInfo("Device created: %p", m_Device.Get());

    auto onDeviceError = [](WGPUErrorType type, const char* message, [[maybe_unused]] void* userdata)
    {
        const char *errorTypeString = wgpu::atom::ErrorTypeToString(type).c_str();
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
    wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
    shaderCodeDesc.code = Reader::ReadTextFile("../../../assets/shaders/shader.wgsl");

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

    std::vector<wgpu::VertexBufferLayout> vertexBufferLayouts(2);
    wgpu::VertexAttribute positionAttrib{};

    positionAttrib.shaderLocation = 0;
    positionAttrib.format = wgpu::VertexFormat::Float32x3;
    positionAttrib.offset = 0;

    vertexBufferLayouts[0].attributeCount = 1;
    vertexBufferLayouts[0].arrayStride = 3 * sizeof(float);
    vertexBufferLayouts[0].attributes = &positionAttrib;
    vertexBufferLayouts[0].stepMode = wgpu::VertexStepMode::Vertex;

    wgpu::VertexAttribute colorAttrib{};

    colorAttrib.shaderLocation = 1;
    colorAttrib.format = wgpu::VertexFormat::Float32x4;
    colorAttrib.offset = 0;

    vertexBufferLayouts[1].attributeCount = 1;
    vertexBufferLayouts[1].arrayStride = 4 * sizeof(float);
    vertexBufferLayouts[1].attributes = &colorAttrib;
    vertexBufferLayouts[1].stepMode = wgpu::VertexStepMode::Vertex;

    pipelineDesc.vertex.bufferCount = static_cast<u32>(vertexBufferLayouts.size());
    pipelineDesc.vertex.buffers = vertexBufferLayouts.data();

    m_RenderPipeline = m_Device.CreateRenderPipeline(&pipelineDesc);
}

void Renderer::SetupBuffers()
{
    
}

wgpu::Device& Renderer::GetDevice()
{
    return m_Device;
}

wgpu::Queue& Renderer::GetQueue()
{
    return m_Queue;
}

}