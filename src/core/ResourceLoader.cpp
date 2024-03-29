#include "ResourceLoader.h"
#include "tiny_gltf.h"
#include "Logger.h"
#include "Engine.h"
#include "Components.h"

#include "stb_image.h"
#include <bit>

namespace atom
{

CMesh ResourceLoader::LoadMesh(const char* path, EModelImportType modelType)
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

            const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
            const tinygltf::Buffer& normalBuffer = model.buffers[normalView.buffer];

            const void* normalData = &normalBuffer.data[normalView.byteOffset + normalAccessor.byteOffset];
            const u32 normalCount = static_cast<u32>(normalAccessor.count);

            meshComponent.normalData.reserve(normalCount * 3);
            for (u32 i = 0; i < normalCount * 3; i += 3)
            {
				meshComponent.normalData.push_back(((float*)normalData)[i + 0]);
				meshComponent.normalData.push_back(((float*)normalData)[i + 1]);
				meshComponent.normalData.push_back(((float*)normalData)[i + 2]);
			}

            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];

            const void* uvData = &uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset];
            const u32 uvCount = static_cast<u32>(uvAccessor.count);

            meshComponent.uvData.reserve(uvCount * 2);
            for (u32 i = 0; i < uvCount * 2; i += 2)
            {
				meshComponent.uvData.push_back(((float*)uvData)[i + 0]);
				meshComponent.uvData.push_back(((float*)uvData)[i + 1]);
			}

            meshComponent.colorData.reserve(vertexCount * 4);
            for (u32 i = 0; i < vertexCount * 4; i += 4)
            {
				meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
                meshComponent.colorData.push_back(1.0f);
			}

            meshComponent.indexCount = (i32)indexCount;
        }
    }

    std::vector<f32>& tangents = meshComponent.tangentData;
    tangents.resize(meshComponent.pointData.size());

    std::vector<f32>& bitangents = meshComponent.bitangentData;
    bitangents.resize(meshComponent.pointData.size());

    for(u32 i = 0; i < meshComponent.indexData.size(); i += 3)
    {
        v3 p1 = v3(meshComponent.pointData[meshComponent.indexData[i + 0] * 3 + 0],
                   meshComponent.pointData[meshComponent.indexData[i + 0] * 3 + 1],
                   meshComponent.pointData[meshComponent.indexData[i + 0] * 3 + 2]);
        v3 p2 = v3(meshComponent.pointData[meshComponent.indexData[i + 1] * 3 + 0],
                   meshComponent.pointData[meshComponent.indexData[i + 1] * 3 + 1],
                   meshComponent.pointData[meshComponent.indexData[i + 1] * 3 + 2]);
        v3 p3 = v3(meshComponent.pointData[meshComponent.indexData[i + 2] * 3 + 0],
                   meshComponent.pointData[meshComponent.indexData[i + 2] * 3 + 1],
                   meshComponent.pointData[meshComponent.indexData[i + 2] * 3 + 2]);

        v2 uv1 = v2(meshComponent.uvData[meshComponent.indexData[i + 0] * 2 + 0],
                    meshComponent.uvData[meshComponent.indexData[i + 0] * 2 + 1]);
        v2 uv2 = v2(meshComponent.uvData[meshComponent.indexData[i + 1] * 2 + 0],
                    meshComponent.uvData[meshComponent.indexData[i + 1] * 2 + 1]);
        v2 uv3 = v2(meshComponent.uvData[meshComponent.indexData[i + 2] * 2 + 0],
                    meshComponent.uvData[meshComponent.indexData[i + 2] * 2 + 1]);

        m3 TBN = CalculateTangent(p1, p2, p3, uv1, uv2, uv3);

        v3& tangent = TBN[0];

        tangents[meshComponent.indexData[i + 0] * 3 + 0] = tangent.x;
        tangents[meshComponent.indexData[i + 0] * 3 + 1] = tangent.y;
        tangents[meshComponent.indexData[i + 0] * 3 + 2] = tangent.z;

        tangents[meshComponent.indexData[i + 1] * 3 + 0] = tangent.x;
        tangents[meshComponent.indexData[i + 1] * 3 + 1] = tangent.y;
        tangents[meshComponent.indexData[i + 1] * 3 + 2] = tangent.z;

        tangents[meshComponent.indexData[i + 2] * 3 + 0] = tangent.x;
        tangents[meshComponent.indexData[i + 2] * 3 + 1] = tangent.y;
        tangents[meshComponent.indexData[i + 2] * 3 + 2] = tangent.z;

        v3& bitangent = TBN[1];
        bitangents[meshComponent.indexData[i + 0] * 3 + 0] = bitangent.x;
        bitangents[meshComponent.indexData[i + 0] * 3 + 1] = bitangent.y;
        bitangents[meshComponent.indexData[i + 0] * 3 + 2] = bitangent.z;

        bitangents[meshComponent.indexData[i + 1] * 3 + 0] = bitangent.x;
        bitangents[meshComponent.indexData[i + 1] * 3 + 1] = bitangent.y;
        bitangents[meshComponent.indexData[i + 1] * 3 + 2] = bitangent.z;

        bitangents[meshComponent.indexData[i + 2] * 3 + 0] = bitangent.x;
        bitangents[meshComponent.indexData[i + 2] * 3 + 1] = bitangent.y;
        bitangents[meshComponent.indexData[i + 2] * 3 + 2] = bitangent.z;
    }

	wgpu::BufferDescriptor bufferDesc;
    bufferDesc.label = "Position Buffer";
	bufferDesc.size = (meshComponent.pointData.size() * sizeof(f32) + 3) & ~3;
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	bufferDesc.mappedAtCreation = false;
    meshComponent.positionBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.positionBuffer, 0, meshComponent.pointData.data(), bufferDesc.size);

    bufferDesc.label = "Normal Buffer";
    bufferDesc.size = (meshComponent.normalData.size() * sizeof(f32) + 3) & ~3;
    meshComponent.normalBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(meshComponent.normalBuffer, 0, meshComponent.normalData.data(), bufferDesc.size);

    bufferDesc.label = "Tangent Buffer";
    bufferDesc.size = (meshComponent.tangentData.size() * sizeof(f32) + 3) & ~3;
    meshComponent.tangentBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(meshComponent.tangentBuffer, 0, meshComponent.tangentData.data(), bufferDesc.size);

    bufferDesc.label = "Bitangent Buffer";
    bufferDesc.size = (meshComponent.bitangentData.size() * sizeof(f32) + 3) & ~3;
    meshComponent.bitangentBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
    Engine::GetQueue().WriteBuffer(meshComponent.bitangentBuffer, 0, meshComponent.bitangentData.data(), bufferDesc.size);

	bufferDesc.size = (meshComponent.colorData.size() * sizeof(f32) + 3) & ~3;
    bufferDesc.label = "Color Buffer";
    meshComponent.colorBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.colorBuffer, 0, meshComponent.colorData.data(), bufferDesc.size);

	bufferDesc.label = "UV Buffer";
	bufferDesc.size = (meshComponent.uvData.size() * sizeof(f32) + 3) & ~3;
	meshComponent.uvBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.uvBuffer, 0, meshComponent.uvData.data(), bufferDesc.size);

	bufferDesc.size = (meshComponent.indexData.size() * sizeof(u16) + 3) & ~3;
    bufferDesc.label = "Index Buffer";
	bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    meshComponent.indexBuffer = Engine::GetDevice().CreateBuffer(&bufferDesc);
	Engine::GetQueue().WriteBuffer(meshComponent.indexBuffer, 0, meshComponent.indexData.data(), bufferDesc.size);

	return meshComponent;
}

wgpu::Texture ResourceLoader::LoadTexture(const char* path, wgpu::Device& device, wgpu::TextureView* pTextureView)
{
	i32 width, height, channels;
	unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4);

    if (!pixels)
    {
		LogError("Failed to load texture: %s\n", path);
		return nullptr;
	}

    wgpu::TextureDescriptor textureDesc;
	textureDesc.dimension = wgpu::TextureDimension::e2D;
	textureDesc.format = wgpu::TextureFormat::RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
	textureDesc.sampleCount = 1;
	textureDesc.size = { (unsigned int)width, (unsigned int)height, 1 };
    textureDesc.mipLevelCount = std::bit_width(std::max(textureDesc.size.width, textureDesc.size.height));
	textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
	textureDesc.viewFormatCount = 0;
	textureDesc.viewFormats = nullptr;
    wgpu::Texture texture = device.CreateTexture(&textureDesc);

	WriteMipMaps(device, texture, textureDesc.size, textureDesc.mipLevelCount, pixels);

    stbi_image_free(pixels);

    if (pTextureView)
    {
		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 1;
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = textureDesc.mipLevelCount;
		textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
		textureViewDesc.format = textureDesc.format;
		*pTextureView = texture.CreateView(&textureViewDesc);
	}

    return texture;
}

void ResourceLoader::WriteMipMaps(const wgpu::Device& device, wgpu::Texture texture, 
    wgpu::Extent3D textureSize, u32 mipLevelCount, const unsigned char* pixelData)
{
	wgpu::Queue queue = device.GetQueue();

	// Arguments telling which part of the texture we upload to
	wgpu::ImageCopyTexture destination;
	destination.texture = texture;
	destination.origin = { 0, 0, 0 };
	destination.aspect = wgpu::TextureAspect::All;

	// Arguments telling how the C++ side pixel memory is laid out
	wgpu::TextureDataLayout source;
	source.offset = 0;

	// Create image data
	wgpu::Extent3D mipLevelSize = textureSize;
	std::vector<unsigned char> previousLevelPixels;
	wgpu::Extent3D previousMipLevelSize;
	for (uint32_t level = 0; level < mipLevelCount; ++level) {
		// Pixel data for the current level
		std::vector<unsigned char> pixels(4 * mipLevelSize.width * mipLevelSize.height);
		if (level == 0) {
			// We cannot really avoid this copy since we need this
			// in previousLevelPixels at the next iteration
			memcpy(pixels.data(), pixelData, pixels.size());
		}
		else {
			// Create mip level data
			for (uint32_t i = 0; i < mipLevelSize.width; ++i) {
				for (uint32_t j = 0; j < mipLevelSize.height; ++j) {
					unsigned char* p = &pixels[4 * (j * mipLevelSize.width + i)];
					// Get the corresponding 4 pixels from the previous level
					unsigned char* p00 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 0))];
					unsigned char* p01 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 1))];
					unsigned char* p10 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 0))];
					unsigned char* p11 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 1))];
					// Average
					p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / 4;
					p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / 4;
					p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / 4;
					p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / 4;
				}
			}
		}

		// Upload data to the GPU texture
		destination.mipLevel = level;
		source.bytesPerRow = 4 * mipLevelSize.width;
		source.rowsPerImage = mipLevelSize.height;
		queue.WriteTexture(&destination, pixels.data(), pixels.size(), &source, &mipLevelSize);

		previousLevelPixels = std::move(pixels);
		previousMipLevelSize = mipLevelSize;
		mipLevelSize.width /= 2;
		mipLevelSize.height /= 2;
	}
}

m3 ResourceLoader::CalculateTangent(const v3 &p1, const v3 &p2, const v3 &p3, const v2 &uv1, const v2 &uv2,
                                    const v2 &uv3)
{
    v3 edge1 = p2 - p1;
    v3 edge2 = p3 - p1;
    v2 deltaUV1 = uv2 - uv1;
    v2 deltaUV2 = uv3 - uv1;

    v3 tangent = glm::normalize(edge1 * deltaUV2.y - edge2 * deltaUV1.y);
    v3 bitangent = glm::normalize(edge2 * deltaUV1.x - edge1 * deltaUV2.x);
    v3 normal = glm::normalize(glm::cross(edge1, edge2));

    return m3(tangent, bitangent, normal);
}

} // namespace atom