#include "System.h"
#include "webgpu/webgpu_cpp.h"

namespace atom
{

class Render3dSystem : public System
{
public:
	void OnFrame(const wgpu::RenderPassEncoder& renderPass) const;
};

} // namespace atom