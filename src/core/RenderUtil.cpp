#include "RenderUtil.h"

namespace atom::RenderUtil
{

u32 CeilToNextMultiple(u32 value, u32 step)
{
	uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
	return step * divide_and_ceil;
}

} // namespace atom