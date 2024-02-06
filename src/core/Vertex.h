#pragma once

#include "../Defines.h"
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>

namespace atom
{

struct FVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 texCoords;

	i32 indexCount;
	wgpu::Buffer positionBuffer;
	std::vector<f32> pointData;

	wgpu::Buffer colorBuffer;
	std::vector<f32> colorData;

	wgpu::Buffer indexBuffer;
	std::vector<u16> indexData;
};

} // namespace atom