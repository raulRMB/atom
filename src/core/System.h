#pragma once

#include "../Defines.h"
#include <concepts>

namespace atom
{

class SSystem
{
public:
	template <std::derived_from<SSystem> T>
	static T& Get();
};

template <std::derived_from<SSystem> T>
T& SSystem::Get()
{
    static T instance;
    return instance;
}

class SUpdateSystem : public SSystem
{
public:
	virtual void Update(f32 dt) = 0;
};

class SRenderSystem : public SSystem
{
public:
	virtual void Render() = 0;
};

} // namespace atom