//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_RENDERER_H
#define ATOM_RENDERER_H

#include "../Defines.h"
#include "dawn/webgpu_cpp.h"

namespace atom
{

class Renderer
{
private:
    wgpu::Instance m_Instance;
    wgpu::Adapter m_Adapter;
    wgpu::Surface m_Surface;
    wgpu::Device m_Device;

    wgpu::Adapter RequestAdapter(wgpu::Instance instance, const wgpu::RequestAdapterOptions* options);
public:
    Renderer();
    ~Renderer();

    void Init();
    void Draw();

    static Renderer* Create();
};

}

#endif //ATOM_RENDERER_H
