#include "../Defines.h"



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

class ModelLoader
{
public:
	static CMesh LoadMesh(const char* path, EModelImportType modelType = EModelImportType::glb);
};

} // namespace atom