//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_ENGINE_H
#define ATOM_ENGINE_H

#include "../Defines.h"
#include "entt/entt.hpp"
#include "Scene.h"
#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace atom
{

class Engine
{
public:
    friend class App;

    Engine();
    ~Engine();

    static Engine& Instance();
private:
    i32 Run(int argc, char** argv);
    static i32 ParseCommandLine(int argc, char** argv);
    i32 Init();
    i32 MainLoop();
    i32 Shutdown();
private:
    class Window* m_pWindow;
    class Renderer* m_pRenderer;
    class Scene* m_pCurrentScene;

    f64 m_CurrentFrameTime;
    f64 m_LastFrameTime;
    f64 m_DeltaTime;
public:
    void UpdateRender();

    [[nodiscard]] Window* GetWindow() const;
    GLFWwindow* GetGLFWWindow();
    [[nodiscard]] Renderer* GetRenderer() const;
    class Scene* GetCurrentScene() const;

    static entt::entity CreateEntity();
    
    template<typename C>
    static void AddComponent(const entt::entity entity, C component)
	{
		Instance().GetCurrentScene()->m_Registry.emplace<C>(entity, component);
	}

    template<typename ...T>
    static auto GetView()
    {
		return Instance().GetCurrentScene()->m_Registry.view<T...>();
    }

    template <typename T>
    static T& GetComponent(const entt::entity entity) 
    { 
        return Instance().GetCurrentScene()->m_Registry.get<T>(entity);
    }

    static wgpu::Device& GetDevice();
    static wgpu::Queue& GetQueue();

    void SetRenderer(Renderer* renderer);
};

}

#endif //ATOM_ENGINE_H
