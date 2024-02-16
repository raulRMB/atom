#include "3dRenderSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Renderer.h"
#include <glm/gtx/transform.hpp>
#include "CTransfrom.h"

namespace atom
{ 

void SRender3d::RenderFrame(const wgpu::RenderPassEncoder& renderPass, wgpu::Queue& queue, const wgpu::Buffer& buffer) const
{
    for (auto& entity : Engine::GetView<CMesh, CTransform>())
    {
		auto& mesh = Engine::GetComponent<CMesh>(entity);
		auto& pos = Engine::GetComponent<CTransform>(entity).Position;

		auto& positionBuffer = mesh.positionBuffer;
		auto& normalBuffer = mesh.normalBuffer;
		auto& uvBuffer = mesh.uvBuffer;
		auto& colorBuffer = mesh.colorBuffer;
		auto& indexBuffer = mesh.indexBuffer;

		auto& pointData = mesh.pointData;
		auto& normalData = mesh.normalData;
		auto& colorData = mesh.colorData;
		auto& uvData = mesh.uvData;

		auto& indexData = mesh.indexData;
		auto& indexCount = mesh.indexCount;

		MyUniforms uniforms;
		m4 model = m4(1.0f);
		model = glm::translate(model, glm::vec3(pos.x, 0.0f, pos.z));
		uniforms.m_Model = model;

		queue.WriteBuffer(buffer, 0, &uniforms, sizeof(MyUniforms::m_Model));

		renderPass.SetVertexBuffer(0, positionBuffer, 0, pointData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(1, normalBuffer, 0, normalData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(2, colorBuffer, 0, colorData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(3, uvBuffer, 0, uvData.size() * sizeof(f32));
		renderPass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, indexData.size() * sizeof(u16));
		renderPass.DrawIndexed(indexCount, 1, 0, 0, 0);
	}
}

} // namespace atom