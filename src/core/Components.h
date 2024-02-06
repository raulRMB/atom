#pragma once

#include "Vertex.h"
#include <vector>

namespace atom
{

struct CComponent
{

};

struct CTransform : public CComponent
{
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};

struct CMesh : public CComponent
{
	const char* Path;
	//std::vector<FVertex> Vertices;
	std::vector<u32> Indices;

	i32 indexCount;
	wgpu::Buffer positionBuffer;
	std::vector<f32> pointData;
	wgpu::Buffer colorBuffer;
	std::vector<f32> colorData;

	wgpu::Buffer indexBuffer;
	std::vector<u16> indexData;

};

} // namespace atom