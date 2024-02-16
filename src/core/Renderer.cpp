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
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <cmath>
#include "RenderUtil.h"
#include "Camera/CCamera.h"

#include "ResourceLoader.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/ext.hpp>

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
    SetupBuffers();
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

    wgpu::RenderPassDepthStencilAttachment depthStencilAttachment{};
    depthStencilAttachment.view = m_DepthTextureView;
    depthStencilAttachment.depthClearValue = 1.0f;
    depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
    depthStencilAttachment.depthReadOnly = false;
    depthStencilAttachment.stencilClearValue = 0;
    depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
    depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
    depthStencilAttachment.stencilReadOnly = true;
    renderPassDesc.depthStencilAttachment = &depthStencilAttachment;
    wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);
    
    renderPass.SetPipeline(m_RenderPipeline);


    //u32 dynamicOffset = 0;

    renderPass.SetBindGroup(0, m_BindGroup, 0, nullptr);

    //float t = static_cast<float>(glfwGetTime());
    //MyUniforms uniforms;
    //uniforms.time = t;
    //uniforms.color = { 1.f, fmod(t, 1.f), 1.f, 1.f};

    MyUniforms uniforms = {};

    Engine::GetView<CCamera>().each([&](const CCamera& camera)
    {
        m4 model = m4(1.0f);
        model = glm::translate(model, v3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (f32)glfwGetTime(), v3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, v3(1.f, 1.f, 1.f));

        uniforms.m_Model = model;
        uniforms.m_View = camera.View;
        uniforms.m_Projection = camera.Projection;
    });

    //m4 model = m4(1.0f);
    ////model *= glm::rotate(m4(1.0f), (f32)glfwGetTime(), v3(0.0f, 1.0f, 0.0f));
    //model = glm::translate(model, v3(0.0f, 0.0f, 0.4f));

    //uniforms.m_Mvp = model;

    m_Queue.WriteBuffer(m_UniformBuffer, 0, &uniforms, sizeof(MyUniforms));
    //m_Queue.WriteBuffer(uniformBuffer, offsetof(MyUniforms, time), &t, sizeof(MyUniforms::time));

    const SRender3d& rend = SSystem::Get<SRender3d>();
    rend.RenderFrame(renderPass, m_Queue, m_UniformBuffer);

    //dynamicOffset = m_UniformStride;
    //renderPass.SetBindGroup(0, m_BindGroup, 1, &dynamicOffset);

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

    wgpu::Limits deviceLimits = supportedLimits.limits;

    m_UniformStride = RenderUtil::CeilToNextMultiple(sizeof(MyUniforms), deviceLimits.minUniformBufferOffsetAlignment);

    wgpu::RequiredLimits requiredLimits = {};
    requiredLimits.limits.maxVertexAttributes = 4;
    requiredLimits.limits.maxVertexBuffers = 1;
    requiredLimits.limits.maxBufferSize = 8 * 7 * sizeof(float);
    requiredLimits.limits.maxVertexBufferArrayStride = 7 * sizeof(float);
    requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
    requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
    requiredLimits.limits.maxInterStageShaderComponents = 8;

    requiredLimits.limits.maxBindGroups = 1;
    requiredLimits.limits.maxUniformBuffersPerShaderStage = 1;
    requiredLimits.limits.maxUniformBufferBindingSize = 16 * 4;

    requiredLimits.limits.maxDynamicUniformBuffersPerPipelineLayout = 1;

    requiredLimits.limits.maxTextureDimension1D = 2048;
    requiredLimits.limits.maxTextureDimension2D = 2048;
    requiredLimits.limits.maxTextureArrayLayers = 1;

    requiredLimits.limits.maxSamplersPerShaderStage = 1;

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
    RenderPipelineResources resources;
    SetupShaderModules(resources, "../../../assets/shaders/shader.wgsl");
    SetupPipelineProperties(resources);
    SetupBindGroupLayout(resources);
    SetupPipelineLayout(resources);
    SetupVertexAttributes(resources);
    SetupVertexBufferLayouts(resources);
    //SetupTexture(resources);
    LoadTexture(resources);
    SetupSampler(resources);
    SetupUniformBuffer(resources);
    SetupDepthStencil(resources);

    m_RenderPipeline = m_Device.CreateRenderPipeline(&resources.pipelineDesc);
}

void Renderer::SetupBuffers()
{
    wgpu::BufferDescriptor bufferDesc{};
    bufferDesc.size = m_UniformStride + sizeof(MyUniforms);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = false;
    m_UniformBuffer = m_Device.CreateBuffer(&bufferDesc);
    MyUniforms uniforms{};
    m_Queue.WriteBuffer(m_UniformBuffer, 0, &uniforms, sizeof(MyUniforms));
}

void Renderer::SetupShaderModules(RenderPipelineResources& resources, const char* path)
{
    resources.shaderCodeDesc.code = Reader::ReadTextFile(path);

#ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;
#endif
    resources.shaderDesc.nextInChain = &resources.shaderCodeDesc;
    resources.shaderModule = m_Device.CreateShaderModule(&resources.shaderDesc);

    resources.pipelineDesc.vertex.bufferCount = 0;
    resources.pipelineDesc.vertex.buffers = nullptr;
    resources.pipelineDesc.vertex.module = resources.shaderModule;
    resources.pipelineDesc.vertex.entryPoint = "vs_main";
    resources.pipelineDesc.vertex.constantCount = 0;
    resources.pipelineDesc.vertex.constants = nullptr;

    resources.fragmentState.module = resources.shaderModule;
    resources.fragmentState.entryPoint = "fs_main";
    resources.fragmentState.constantCount = 0;
    resources.fragmentState.constants = nullptr;
    resources.pipelineDesc.fragment = &resources.fragmentState;

    resources.colorTarget.format = m_SwapChain.GetCurrentTexture().GetFormat();

    resources.blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    resources.blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    resources.blendState.color.operation = wgpu::BlendOperation::Add;

    resources.colorTarget.blend = &resources.blendState;
    resources.colorTarget.writeMask = wgpu::ColorWriteMask::All; // We could write to only some of the color channels.

    resources.fragmentState.targetCount = 1;
    resources.fragmentState.targets = &resources.colorTarget;
}

void Renderer::SetupPipelineProperties(RenderPipelineResources& resources)
{
    resources.pipelineDesc.multisample.count = 1;
    resources.pipelineDesc.multisample.mask = ~0u;
    resources.pipelineDesc.multisample.alphaToCoverageEnabled = false;

    resources.pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    resources.pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    resources.pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    resources.pipelineDesc.primitive.cullMode = wgpu::CullMode::None;
}

void Renderer::SetupBindGroupLayout(RenderPipelineResources& resources)
{
    resources.bindingLayoutEntries.resize(3, {});

    resources.bindingLayoutEntries[0].binding = 0;
    resources.bindingLayoutEntries[0].buffer.hasDynamicOffset = false;
    resources.bindingLayoutEntries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    resources.bindingLayoutEntries[0].buffer.type = wgpu::BufferBindingType::Uniform;
    resources.bindingLayoutEntries[0].buffer.minBindingSize = sizeof(MyUniforms);

    resources.bindingLayoutEntries[1].binding = 1;
    resources.bindingLayoutEntries[1].visibility = wgpu::ShaderStage::Fragment;
    resources.bindingLayoutEntries[1].texture.sampleType = wgpu::TextureSampleType::Float;
    resources.bindingLayoutEntries[1].texture.viewDimension = wgpu::TextureViewDimension::e2D;
    resources.bindingLayoutEntries[1].texture.multisampled = false;
    
    resources.bindingLayoutEntries[2].binding = 2;
    resources.bindingLayoutEntries[2].visibility = wgpu::ShaderStage::Fragment;
    resources.bindingLayoutEntries[2].sampler.type = wgpu::SamplerBindingType::Filtering;

    resources.bindGroupLayoutDesc.entryCount = resources.bindingLayoutEntries.size();
    resources.bindGroupLayoutDesc.entries = resources.bindingLayoutEntries.data();
    resources.bindGroupLayout = m_Device.CreateBindGroupLayout(&resources.bindGroupLayoutDesc);
}

void Renderer::SetupPipelineLayout(RenderPipelineResources& resources)
{
    resources.pipelineLayoutDesc.bindGroupLayoutCount = 1;
    resources.pipelineLayoutDesc.bindGroupLayouts = &resources.bindGroupLayout;
    resources.pipelineLayout = m_Device.CreatePipelineLayout(&resources.pipelineLayoutDesc);
    resources.pipelineDesc.layout = resources.pipelineLayout;
}

void Renderer::SetupVertexBufferLayouts(RenderPipelineResources& resources)
{
    resources.vertexBufferLayouts.resize(4, {});
    
    // Position attribute
    resources.vertexBufferLayouts[0].attributeCount = 1;
    resources.vertexBufferLayouts[0].arrayStride = 3 * sizeof(float);
    resources.vertexBufferLayouts[0].attributes = &resources.vertexAttributes[0];
    resources.vertexBufferLayouts[0].stepMode = wgpu::VertexStepMode::Vertex;

    // Normal attribute
    resources.vertexBufferLayouts[1].attributeCount = 1;
    resources.vertexBufferLayouts[1].arrayStride = 3 * sizeof(float);
    resources.vertexBufferLayouts[1].attributes = &resources.vertexAttributes[1];
    resources.vertexBufferLayouts[1].stepMode = wgpu::VertexStepMode::Vertex;

    // Color attribute
    resources.vertexBufferLayouts[2].attributeCount = 1;
    resources.vertexBufferLayouts[2].arrayStride = 4 * sizeof(float);
    resources.vertexBufferLayouts[2].attributes = &resources.vertexAttributes[2];
    resources.vertexBufferLayouts[2].stepMode = wgpu::VertexStepMode::Vertex;

    // UV attribute
    resources.vertexBufferLayouts[3].attributeCount = 1;
    resources.vertexBufferLayouts[3].arrayStride = 2 * sizeof(float);
    resources.vertexBufferLayouts[3].attributes = &resources.vertexAttributes[3];
    resources.vertexBufferLayouts[3].stepMode = wgpu::VertexStepMode::Vertex;

    resources.pipelineDesc.vertex.bufferCount = static_cast<u32>(resources.vertexBufferLayouts.size());
    resources.pipelineDesc.vertex.buffers = resources.vertexBufferLayouts.data();
}

void Renderer::SetupVertexAttributes(RenderPipelineResources& resources)
{
    resources.vertexAttributes.resize(4, {});

    // Position attribute
    resources.vertexAttributes[0].shaderLocation = 0;
    resources.vertexAttributes[0].format = wgpu::VertexFormat::Float32x3;
    resources.vertexAttributes[0].offset = 0;

    // Normal attribute
    resources.vertexAttributes[1].shaderLocation = 1;
    resources.vertexAttributes[1].format = wgpu::VertexFormat::Float32x3;
    resources.vertexAttributes[1].offset = 0;

    // Color attribute
	resources.vertexAttributes[2].shaderLocation = 2;
	resources.vertexAttributes[2].format = wgpu::VertexFormat::Float32x4;
	resources.vertexAttributes[2].offset = 0;

	// UV attribute
	resources.vertexAttributes[3].shaderLocation = 3;
	resources.vertexAttributes[3].format = wgpu::VertexFormat::Float32x2;
	resources.vertexAttributes[3].offset = 0;
}

void Renderer::SetupUniformBuffer(RenderPipelineResources& resources)
{
    resources.bindGroupEntry.resize(3, {});

    resources.bindGroupEntry[0].binding = 0;
    resources.bindGroupEntry[0].buffer = m_UniformBuffer;
    resources.bindGroupEntry[0].offset = 0;
    resources.bindGroupEntry[0].size = sizeof(MyUniforms);

    resources.bindGroupEntry[1].binding = 1;
    resources.bindGroupEntry[1].textureView = resources.textureView;

    resources.bindGroupEntry[2].binding = 2;
    resources.bindGroupEntry[2].sampler = resources.sampler;

    resources.bindGroupDesc.layout = resources.bindGroupLayout;
    resources.bindGroupDesc.entryCount = (u32)resources.bindGroupEntry.size();
    resources.bindGroupDesc.entries = resources.bindGroupEntry.data();
    m_BindGroup = m_Device.CreateBindGroup(&resources.bindGroupDesc);
}

void Renderer::SetupDepthStencil(RenderPipelineResources& resources)
{
    resources.depthStencilState.depthWriteEnabled = true;
    resources.depthStencilState.depthCompare = wgpu::CompareFunction::Less;
    resources.depthStencilState.stencilReadMask = 0;
    resources.depthStencilState.stencilWriteMask = 0;
    resources.depthStencilState.format = m_DepthTextureFormat;
    resources.pipelineDesc.depthStencil = &resources.depthStencilState;

    resources.depthTextureDesc.dimension = wgpu::TextureDimension::e2D;
    resources.depthTextureDesc.format = m_DepthTextureFormat;
    resources.depthTextureDesc.mipLevelCount = 1;
    resources.depthTextureDesc.sampleCount = 1;
    resources.depthTextureDesc.size = { 640, 480, 1 };
    resources.depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
    resources.depthTextureDesc.viewFormatCount = 1;
    resources.depthTextureDesc.viewFormats = &m_DepthTextureFormat;
    resources.depthTexture = m_Device.CreateTexture(&resources.depthTextureDesc);

    resources.depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
    resources.depthTextureViewDesc.baseArrayLayer = 0;
    resources.depthTextureViewDesc.arrayLayerCount = 1;
    resources.depthTextureViewDesc.baseMipLevel = 0;
    resources.depthTextureViewDesc.mipLevelCount = 1;
    resources.depthTextureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
    resources.depthTextureViewDesc.format = m_DepthTextureFormat;
    m_DepthTextureView = resources.depthTexture.CreateView(&resources.depthTextureViewDesc);
}

void Renderer::SetupTexture(RenderPipelineResources& resources)
{
    wgpu::TextureDescriptor& textureDesc = resources.textureDesc;
    textureDesc.dimension = wgpu::TextureDimension::e2D;
    textureDesc.size = { 256, 256, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormatCount = 0;
    textureDesc.viewFormats = nullptr;

	std::vector<uint8_t> pixels(4 * textureDesc.size.width * textureDesc.size.height);
	for (uint32_t i = 0; i < textureDesc.size.width; ++i) {
		for (uint32_t j = 0; j < textureDesc.size.height; ++j) {
			uint8_t* p = &pixels[4 * (j * textureDesc.size.width + i)];
			p[0] = (i / 16) % 2 == (j / 16) % 2 ? 255 : 0; // r
			p[1] = ((i - j) / 16) % 2 == 0 ? 255 : 0; // g
			p[2] = ((i + j) / 16) % 2 == 0 ? 255 : 0; // b
			p[3] = 255; // a
		}
	}

    resources.texture = m_Device.CreateTexture(&textureDesc);

    wgpu::TextureViewDescriptor textureViewDesc{};
    textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
    textureViewDesc.format = textureDesc.format;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    resources.textureView = resources.texture.CreateView(&textureViewDesc);

	wgpu::ImageCopyTexture destination{};
    destination.texture = resources.texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = wgpu::TextureAspect::All;

	wgpu::TextureDataLayout source{};
    source.offset = 0;
    source.bytesPerRow = 4 * textureDesc.size.width;
    source.rowsPerImage = textureDesc.size.height;

    m_Queue.WriteTexture(&destination, pixels.data(), pixels.size(), &source, &textureDesc.size);
}

void Renderer::LoadTexture(RenderPipelineResources& resources)
{
    resources.texture = ResourceLoader::LoadTexture("../../../assets/textures/bricks.png", m_Device, &resources.textureView);
	if (!resources.texture)
    {
        LogError("Could not load texture!");
	}
}

void Renderer::SetupSampler(RenderPipelineResources& resources)
{
    resources.samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
	resources.samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
	resources.samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
	resources.samplerDesc.magFilter = wgpu::FilterMode::Linear;
	resources.samplerDesc.minFilter = wgpu::FilterMode::Linear;
	resources.samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
	resources.samplerDesc.lodMinClamp = 0.0f;
	resources.samplerDesc.lodMaxClamp = 1.0f;
	resources.samplerDesc.compare = wgpu::CompareFunction::Undefined;
	resources.samplerDesc.maxAnisotropy = 1;
	resources.sampler = m_Device.CreateSampler(&resources.samplerDesc);
}

wgpu::Device& Renderer::GetDevice()
{
    return m_Device;
}

wgpu::Queue& Renderer::GetQueue()
{
    return m_Queue;
}

} // namespace atom