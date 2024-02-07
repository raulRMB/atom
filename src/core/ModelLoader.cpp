#include "ModelLoader.h"
#include "tiny_gltf.h"
#include "Logger.h"
#include "Engine.h"
#include "Components.h"

namespace atom
{

CMesh ModelLoader::LoadMesh(const char* path)
{
    CMesh meshComponent;

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }
    else
    {
        printf("Loaded glTF\n");
    }

    // Extract vertex and index data from glTF

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            const void* indexData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
            const uint32_t indexCount = static_cast<uint32_t>(accessor.count);

            meshComponent.indexData.resize(indexCount);
            memcpy(meshComponent.indexData.data(), indexData, indexCount * sizeof(uint16_t));

            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& positionView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer& positionBuffer = model.buffers[positionView.buffer];

            const void* positionData = &positionBuffer.data[positionView.byteOffset + positionAccessor.byteOffset];
            const uint32_t vertexCount = static_cast<uint32_t>(positionAccessor.count);

            meshComponent.pointData.resize(vertexCount);

            /*const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("COLOR")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];*/

            //const void* uvData = &uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset];

            /*u32 i = 0;
            for (auto& vertex : meshComponent.pointData)
            {
                vertex = ((float*)positionData)[i + 0] * 0.01f;
            }*/

            for (u32 i = 0; i < vertexCount; i += 7)
            {
                float x = ((float*)positionData)[i + 0] * .1f;
                float y = ((float*)positionData)[i + 1] * .1f;
                float z = 0.f;

				meshComponent.pointData[i + 0] = x;
				meshComponent.pointData[i + 1] = y;
				meshComponent.pointData[i + 2] = z;

                meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(0.0f);
                meshComponent.colorData.push_back(0.0f);
                meshComponent.colorData.push_back(1.0f);
			}
            //                std::cout << i << std::endl;
            //                memcpy(vertices.data(), positionData, vertexCount * sizeof(PosNormalVertex));

            meshComponent.indexCount = indexCount;
        }
    }

	wgpu::BufferDescriptor bufferDesc;
	bufferDesc.size = meshComponent.pointData.size() * sizeof(f32);
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	bufferDesc.mappedAtCreation = false;
    meshComponent.positionBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.positionBuffer, 0, meshComponent.pointData.data(), bufferDesc.size);

	bufferDesc.size = meshComponent.colorData.size() * sizeof(f32);
    meshComponent.colorBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.colorBuffer, 0, meshComponent.colorData.data(), bufferDesc.size);

	bufferDesc.size = meshComponent.indexData.size() * sizeof(u16);
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    meshComponent.indexBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.indexBuffer, 0, meshComponent.indexData.data(), bufferDesc.size);

	return meshComponent;
}

} // namespace atom