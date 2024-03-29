//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_RENDERER_H
#define ATOM_RENDERER_H

#include "../Defines.h"

#include <vector>
#include <webgpu/webgpu_cpp.h>

#include <webgpu/webgpu_glfw.h>

namespace atom
{

struct MyUniforms
{
	m4 m_Model;
	m4 m_View;
	m4 m_Projection;
    v3 m_CameraPosition;
    f32 m_padding;
};

struct RenderPipelineResources
{
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc{};
    wgpu::ShaderModuleDescriptor shaderDesc{};
    wgpu::ShaderModule shaderModule{};
    wgpu::FragmentState fragmentState{};
    wgpu::ColorTargetState colorTarget{};
    wgpu::BlendState blendState{};

    std::vector<wgpu::BindGroupLayoutEntry> bindingLayoutEntries{};
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
    wgpu::BindGroupLayout bindGroupLayout{};

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc{};
    wgpu::PipelineLayout pipelineLayout{};

    std::vector<wgpu::VertexBufferLayout> vertexBufferLayouts{};
    std::vector<wgpu::VertexAttribute> vertexAttributes{};

    std::vector<wgpu::BindGroupEntry> bindGroupEntry{};
    wgpu::BindGroupDescriptor bindGroupDesc{};

    wgpu::DepthStencilState depthStencilState{};
    wgpu::TextureDescriptor depthTextureDesc{};
    wgpu::Texture depthTexture{};
    wgpu::TextureViewDescriptor depthTextureViewDesc{};

    wgpu::Texture baseColorTexture{};
    wgpu::TextureView baseColorTextureView{};

    wgpu::Texture normalTexture{};
    wgpu::TextureView normalTextureView{};

    wgpu::Texture roughnessTexture{};
    wgpu::TextureView roughnessTextureView{};

    wgpu::Texture metallicTexture{};
    wgpu::TextureView metallicTextureView{};

    wgpu::SamplerDescriptor samplerDesc{};
    wgpu::Sampler sampler{};
};

class Renderer
{
private:
    wgpu::Instance m_Instance;
    wgpu::Surface m_Surface;
    wgpu::Adapter m_Adapter;
    wgpu::Device m_Device;
    wgpu::Queue m_Queue;
    wgpu::SwapChain m_SwapChain;
    wgpu::RenderPipeline m_RenderPipeline;

    wgpu::BindGroup m_BindGroup;
    wgpu::Buffer m_UniformBuffer;
    u32 m_UniformStride;
    wgpu::TextureFormat m_DepthTextureFormat = wgpu::TextureFormat::Depth16Unorm;
    wgpu::TextureView m_DepthTextureView;
public:
    Renderer();
    ~Renderer();

    void Init();
    void Draw();

    static Renderer* Create();

    void SetupInstance();
    void SetupSurface();
    void SetupAdapter();
    void SetupDevice();
    void SetupQueue();
    void SetupSwapChain();
    void SetupRenderPipeline();
    void SetupBuffers();

    void SetupShaderModules(RenderPipelineResources& resources, const char* path);
    void SetupPipelineProperties(RenderPipelineResources& resources);
    void SetupBindGroupLayout(RenderPipelineResources& resources);
    void SetupPipelineLayout(RenderPipelineResources& resources);
    void SetupVertexBufferLayouts(RenderPipelineResources& resources);
    void SetupVertexAttributes(RenderPipelineResources& resources);
    void SetupUniformBuffer(RenderPipelineResources& resources);
    void SetupDepthStencil(RenderPipelineResources& resources);
    void SetupTexture(RenderPipelineResources& resources);
    void LoadTextures(RenderPipelineResources& resources);
	void SetupSampler(RenderPipelineResources& resources);

    wgpu::Device& GetDevice();
    wgpu::Queue& GetQueue();
    wgpu::Instance& GetInstance();
};

}

#endif //ATOM_RENDERER_H
