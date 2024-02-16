#pragma once

#include "entt/entt.hpp"
#include "../Defines.h"
#include "System.h"
#include "Logger.h"

namespace atom
{

class Scene
{
friend class Engine;

private:
	entt::registry m_Registry{};

	std::vector<SUpdateSystem*> m_UpdateSystems;
	std::vector<SRenderSystem*> m_RenderSystems;
private:
	Scene();

	void Update(f32 dt);

	void AddUpdateSystem(SUpdateSystem* system);
	void AddRenderSystem(SRenderSystem* system);
};

} // namespace atom