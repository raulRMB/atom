#include "../Defines.h"
#include "webgpu/webgpu_cpp.h"

namespace atom
{

struct CMesh;

enum class EModelImportType
{
	glb,
	gltf,
	obj,
	fbx,
	max,
	blender,
	unknown
};

class ResourceLoader
{
public:
	static CMesh LoadMesh(const char* path, EModelImportType modelType = EModelImportType::glb);
	static wgpu::Texture LoadTexture(const char* path, wgpu::Device& device, wgpu::TextureView* pTextureView = nullptr);
private:
	static void WriteMipMaps(const wgpu::Device& device, wgpu::Texture texture,
		wgpu::Extent3D textureSize, [[maybe_unused]] u32 mipLevelCount, // not used yet
		const unsigned char* pixelData);
};

} // namespace atom