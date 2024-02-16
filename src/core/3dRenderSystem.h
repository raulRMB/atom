#include "System.h"
#include "webgpu/webgpu_cpp.h"

namespace atom
{

class SRender3d : public SSystem
{
public:
	void RenderFrame(const wgpu::RenderPassEncoder& renderPass, wgpu::Queue& queue, const wgpu::Buffer& buffer) const;
};

} // namespace atom