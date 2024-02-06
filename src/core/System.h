#pragma once

#include "../Defines.h"
#include <concepts>

namespace atom
{

class System
{
public:
	template <std::derived_from<System> T>
	static T& Get();
};

template <std::derived_from<System> T>
T& System::Get()
{
    static T instance;
    return instance;
}

} // namespace atom