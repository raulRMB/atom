//
// Created by Raul Romero on 2024-01-13.
//

#include <cassert>
#include "Renderer.h"
#include "Logger.h"
#include "Engine.h"
#include <webgpu/webgpu_glfw.h>

namespace atom
{

Renderer::Renderer() : m_Instance(nullptr), m_Surface(nullptr), m_Adapter(nullptr), m_Device(nullptr)
{

}

Renderer::~Renderer() = default;

void Renderer::Init()
{
    wgpu::InstanceDescriptor instanceDesc = {};
    instanceDesc.nextInChain = nullptr;

    m_Instance = wgpu::CreateInstance(&instanceDesc);

    if(m_Instance == nullptr)
    {
        LogError("Failed to create WebGPU instance.");
        return;
    }

    m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, Engine::Instance().GetGLFWWindow());

    wgpu::RequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = m_Surface;

    m_Adapter = RequestAdapter(m_Instance, &adapterOpts);
}

void Renderer::Draw()
{

}

wgpu::Adapter Renderer::RequestAdapter(wgpu::Instance instance, const wgpu::RequestAdapterOptions* options)
{
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    } userData;

    // Callback called by wgpuInstanceRequestAdapter when the request returns
    // This is a C++ lambda function, but could be any function defined in the
    // global scope. It must be non-capturing (the brackets [] are empty) so
    // that it behaves like a regular C function pointer, which is what
    // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
    // is to convey what we want to capture through the pUserData pointer,
    // provided as the last argument of wgpuInstanceRequestAdapter and received
    // by the callback as its last argument.
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            LogError("Could not get WebGPU adapter: ", message);
        }
        userData.requestEnded = true;
    };

    // Call to the WebGPU request adapter procedure
    instance.RequestAdapter(options, onAdapterRequestEnded, (void*)&userData);

    // In theory we should wait until onAdapterReady has been called, which
    // could take some time (what the 'await' keyword does in the JavaScript
    // code). In practice, we know that when the wgpuInstanceRequestAdapter()
    // function returns its callback has been called.
    assert(userData.requestEnded);

    return userData.adapter;
}

Renderer *Renderer::Create()
{
    Renderer* pRenderer = new Renderer();
    pRenderer->Init();
    return pRenderer;
}

}