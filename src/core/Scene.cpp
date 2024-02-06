#include "Scene.h"
#include "Components.h"
#include "Logger.h"
#include "Engine.h"

namespace atom
{

Scene::Scene() : m_Registry()
{
	auto ent = m_Registry.create();

	CMesh mesh;
	mesh.indexData = {
            0, 1, 2, // Triangle #0
            0, 2, 3  // Triangle #1
    };
    mesh.indexCount = static_cast<int>(mesh.indexData.size());
	mesh.pointData = {
        -0.2f + .3f, -0.2f, // A
        +0.2f + .3f, -0.2f,
        +0.2f + .3f, +0.2f, // C
        -0.2f + .3f, +0.2f,
    };
	mesh.colorData = {
        1.0f, 0.0f, 0.0f, 1.0f, // A
        0.0f, 1.0f, 0.0f, 1.0f, // B
        0.0f, 0.0f, 1.0f, 1.0f, // C
        1.0f, 1.0f, 0.0f, 1.0f, // D
    };
	mesh.Path = "default";

    mesh.colorBuffer = nullptr;
    mesh.indexBuffer = nullptr;

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = mesh.pointData.size() * sizeof(f32);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    mesh.positionBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.positionBuffer, 0, mesh.pointData.data(), bufferDesc.size);

    bufferDesc.size = mesh.colorData.size() * sizeof(f32);
    mesh.colorBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.colorBuffer, 0, mesh.colorData.data(), bufferDesc.size);

    bufferDesc.size = mesh.indexData.size() * sizeof(u16);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    mesh.indexBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.indexBuffer, 0, mesh.indexData.data(), bufferDesc.size);

	m_Registry.emplace<CMesh>(ent, mesh);

    ent = m_Registry.create();

    mesh.indexData = {
            0, 1, 2, // Triangle #0
            0, 2, 3  // Triangle #1
    };
    mesh.indexCount = static_cast<int>(mesh.indexData.size());
    mesh.pointData = {
        -0.2f - .2f, -0.2f, // A
        +0.2f - .2f, -0.2f,
        +0.2f - .2f, +0.2f, // C
        -0.2f - .2f, +0.2f,
    };
    mesh.colorData = {
        1.0f, 0.0f, 0.0f, 1.0f, // A
        1.0f, 1.0f, 0.0f, 1.0f, // B
        1.0f, 0.0f, 1.0f, 1.0f, // C
        1.0f, 1.0f, 0.0f, 1.0f, // D
    };
    mesh.Path = "default";

    mesh.colorBuffer = nullptr;
    mesh.indexBuffer = nullptr;

    bufferDesc.size = mesh.pointData.size() * sizeof(float);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    mesh.positionBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.positionBuffer, 0, mesh.pointData.data(), bufferDesc.size);

    bufferDesc.size = mesh.colorData.size() * sizeof(float);
    mesh.colorBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.colorBuffer, 0, mesh.colorData.data(), bufferDesc.size);

    bufferDesc.size = mesh.indexData.size() * sizeof(u16);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    mesh.indexBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(mesh.indexBuffer, 0, mesh.indexData.data(), bufferDesc.size);

    m_Registry.emplace<CMesh>(ent, mesh);

}

}