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
	// Engine::GetComponent<CTransform>(Engine::GetMainCamera()).Position;

    for (auto& entity : Engine::GetView<CMesh, CTransform>())
    {
		auto& mesh = Engine::GetComponent<CMesh>(entity);
		v3& pos = Engine::GetComponent<CTransform>(entity).Position;
		v3& rot = Engine::GetComponent<CTransform>(entity).Rotation;
		v3& scale = Engine::GetComponent<CTransform>(entity).Scale;

		auto& positionBuffer = mesh.positionBuffer;
		auto& normalBuffer = mesh.normalBuffer;
        auto& tangentBuffer = mesh.tangentBuffer;
        auto& bitangentBuffer = mesh.bitangentBuffer;
		auto& uvBuffer = mesh.uvBuffer;
		auto& colorBuffer = mesh.colorBuffer;
		auto& indexBuffer = mesh.indexBuffer;

		auto& pointData = mesh.pointData;
		auto& normalData = mesh.normalData;
        auto& tangentData = mesh.tangentData;
        auto& bitangentData = mesh.bitangentData;
		auto& colorData = mesh.colorData;
		auto& uvData = mesh.uvData;

		auto& indexData = mesh.indexData;
		auto& indexCount = mesh.indexCount;

		MyUniforms uniforms;
		m4 model = m4(1.0f);
		model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		uniforms.m_Model = model;

		// uniforms.m_CameraPosition = Engine::GetMainCameraPosition();

		queue.WriteBuffer(buffer, 0, &uniforms, sizeof(MyUniforms::m_Model));

		renderPass.SetVertexBuffer(0, positionBuffer, 0, pointData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(1, normalBuffer, 0, normalData.size() * sizeof(f32));
        renderPass.SetVertexBuffer(2, tangentBuffer, 0, tangentData.size() * sizeof(f32));
        renderPass.SetVertexBuffer(3, bitangentBuffer, 0, bitangentData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(4, colorBuffer, 0, colorData.size() * sizeof(f32));
		renderPass.SetVertexBuffer(5, uvBuffer, 0, uvData.size() * sizeof(f32));
		renderPass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, indexData.size() * sizeof(u16));
		renderPass.DrawIndexed(indexCount, 1, 0, 0, 0);
	}
}

} // namespace atom