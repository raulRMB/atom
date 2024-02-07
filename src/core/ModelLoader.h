#include "../Defines.h"



namespace atom
{

struct CMesh;

class ModelLoader
{
public:
	static CMesh LoadMesh(const char* path);
};

}