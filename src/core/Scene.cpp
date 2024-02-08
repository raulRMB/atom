#include "Scene.h"
#include "Components.h"
#include "Logger.h"
#include "Engine.h"
#include "ModelLoader.h"

namespace atom
{

Scene::Scene() : m_Registry()
{
    auto ent = m_Registry.create();
    CMesh mesh = ModelLoader::LoadMesh("../../../assets/models/su.glb", EModelImportType::glb);
    m_Registry.emplace<CMesh>(ent, mesh);
}

}