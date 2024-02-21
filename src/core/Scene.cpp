#include "Scene.h"
#include "Components.h"
#include "Logger.h"
#include "Engine.h"
#include "ResourceLoader.h"
#include "Camera/CCamera.h"
#include "Camera/SCamera.h"
#include "CTransfrom.h"
#include "InputHandler.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/ext.hpp>

namespace atom
{

Scene::Scene()
{
	CMesh mesh = ResourceLoader::LoadMesh("../assets/models/cube.glb", EModelImportType::glb);

	entt::entity ent = m_Registry.create();
	CTransform transform{};
//	transform.Position = v3(0.f, -4.0f, 0.f);
//	transform.Rotation = v3(90.f, 0.f, 90.f);
	transform.Scale = v3(1.f);
	m_Registry.emplace<CTransform>(ent, transform);
	m_Registry.emplace<CMesh>(ent, mesh);

	/*for (i32 i = 0; i < 10; i++)
	{
		for (i32 j = 0; j < 10; j++)
		{
			entt::entity ent = m_Registry.create();
			CTransform transform{};
			transform.Position = glm::vec3(i * 0.5f, 0.0f, j * 0.5f);
			m_Registry.emplace<CTransform>(ent, transform);
			m_Registry.emplace<CMesh>(ent, mesh);
		}
	}*/
}

void Scene::Update(f32 dt)
{
    for (auto system : m_UpdateSystems)
    {
		system->Update(dt);
    }
}

void Scene::AddUpdateSystem(SUpdateSystem* system)
{
	m_UpdateSystems.push_back(system);
}

void Scene::AddRenderSystem(SRenderSystem* system)
{
	m_RenderSystems.push_back(system);
}

} // namespace atom