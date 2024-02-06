#include "3dRenderSystem.h"
#include "Engine.h"
#include "Components.h"

namespace atom
{ 

void Render3dSystem::OnFrame(const wgpu::RenderPassEncoder& renderPass) const
{
    for (auto& entity : Engine::GetView<CMesh>())
    {
		auto& mesh = Engine::GetComponent<CMesh>(entity);
		auto& positionBuffer = mesh.positionBuffer;
		auto& colorBuffer = mesh.colorBuffer;
		auto& indexBuffer = mesh.indexBuffer;
		auto& pointData = mesh.pointData;
		auto& colorData = mesh.colorData;
		auto& indexData = mesh.indexData;
		auto& indexCount = mesh.indexCount;

		renderPass.SetVertexBuffer(0, positionBuffer, 0, pointData.size() * sizeof(float));
		renderPass.SetVertexBuffer(1, colorBuffer, 0, colorData.size() * sizeof(float));
		renderPass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, indexData.size() * sizeof(u16));
		renderPass.DrawIndexed(indexCount, 1, 0, 0, 0);
	}
}

} // namespace atom