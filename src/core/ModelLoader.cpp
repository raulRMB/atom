#include "ModelLoader.h"
#include "tiny_gltf.h"
#include "Logger.h"
#include "Engine.h"
#include "Components.h"

namespace atom
{

CMesh ModelLoader::LoadMesh(const char* path, EModelImportType modelType)
{
    CMesh meshComponent;

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool ret = false;
    switch (modelType)
    {
    case EModelImportType::glb:
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
        break;
    case EModelImportType::gltf:
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        break;
    default:
        LogError("Unknown model type\n");
        return meshComponent;
    }

    if (!warn.empty())
        LogWarning(warn.c_str());

    if (!err.empty())
        LogError(err.c_str());

    if (!ret)
        LogError("Failed to parse glTF\n");

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            const void* indexData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
            const u32 indexCount = static_cast<u32>(accessor.count);

            meshComponent.indexData.resize(indexCount);
            memcpy(meshComponent.indexData.data(), indexData, indexCount * sizeof(u16));

            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& positionView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer& positionBuffer = model.buffers[positionView.buffer];

            const void* positionData = &positionBuffer.data[positionView.byteOffset + positionAccessor.byteOffset];
            const uint32_t vertexCount = static_cast<uint32_t>(positionAccessor.count);

            meshComponent.pointData.reserve(vertexCount * 3);
            for (u32 i = 0; i < vertexCount * 3; i += 3)
            {
				meshComponent.pointData.push_back(((float*)positionData)[i + 0]);
				meshComponent.pointData.push_back(((float*)positionData)[i + 1]);
				meshComponent.pointData.push_back(((float*)positionData)[i + 2]);
			}

            /*const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("COLOR")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];*/

            //const void* uvData = &uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset];

            /*u32 i = 0;
            for (auto& vertex : meshComponent.pointData)
            {
                vertex = ((float*)positionData)[i + 0] * 0.01f;
            }*/

            
            for (u32 i = 0; i < vertexCount * 4; i += 4)
            {
				meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
			}

            meshComponent.indexCount = indexCount;
        }
    }

	wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "Position Buffer";
	bufferDesc.size = (meshComponent.pointData.size() * sizeof(f32) + 3) & ~3;
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	bufferDesc.mappedAtCreation = false;
    meshComponent.positionBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.positionBuffer, 0, meshComponent.pointData.data(), bufferDesc.size);

	bufferDesc.size = (meshComponent.colorData.size() * sizeof(f32) + 3) & ~3;
    bufferDesc.label = "Color Buffer";
    meshComponent.colorBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.colorBuffer, 0, meshComponent.colorData.data(), bufferDesc.size);

	bufferDesc.size = (meshComponent.indexData.size() * sizeof(u16) + 3) & ~3;
    bufferDesc.label = "Index Buffer";
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    meshComponent.indexBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.indexBuffer, 0, meshComponent.indexData.data(), bufferDesc.size);

	return meshComponent;
}

} // namespace atom