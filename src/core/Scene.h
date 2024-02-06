#pragma once

#include "entt/entt.hpp"

namespace atom
{

class Scene
{
friend class Engine;

private:
	entt::registry m_Registry;

private:
	Scene();
};

} // namespace atom