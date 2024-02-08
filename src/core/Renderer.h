//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_RENDERER_H
#define ATOM_RENDERER_H

#include "../Defines.h"
#include <dawn/webgpu_cpp.h>

namespace atom
{

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

    wgpu::Device& GetDevice();
    wgpu::Queue& GetQueue();

    u32 CeilToNextMultiple(u32 value, u32 step) const;
};

}

#endif //ATOM_RENDERER_H
