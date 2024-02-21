#pragma once

#include "Vertex.h"
#include <vector>

namespace atom
{

struct CComponent
{

};

struct CMesh : public CComponent
{
	const char* Path;
	i32 indexCount;
	wgpu::Buffer positionBuffer;
	std::vector<f32> pointData{};
	wgpu::Buffer normalBuffer;
	std::vector<f32> normalData{};
    wgpu::Buffer tangentBuffer;
    std::vector<f32> tangentData{};
    wgpu::Buffer bitangentBuffer;
    std::vector<f32> bitangentData{};
	wgpu::Buffer colorBuffer;
	std::vector<f32> colorData{};
	wgpu::Buffer uvBuffer;
	std::vector<f32> uvData{};

	wgpu::Buffer indexBuffer;
	std::vector<u16> indexData{};

};

} // namespace atom