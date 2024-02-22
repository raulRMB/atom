//
// Created by Raul Romero on 2024-01-13.
//

#include <string>
#include "Engine.h"
#include "Renderer.h"
#include "Window.h"
#include "Logger.h"
#include "Scene.h"
#include "InputHandler.h"
#include "Camera/SCamera.h"

namespace atom
{

Engine::Engine()
{

}

Engine::~Engine()
{

}

Engine& Engine::Instance()
{
    static Engine instance;
    return instance;
}

i32 Engine::Run(int argc, char **argv)
{
    ATOM_ATTEMPT(ParseCommandLine(argc, argv))
    ATOM_ATTEMPT(Init())
    ATOM_ATTEMPT(MainLoop())
    ATOM_ATTEMPT(Shutdown())
    return EXIT_SUCCESS;
}

i32 Engine::ParseCommandLine(int argc, char **argv)
{
    std::string argList;
    for(i32 i = 1; i < argc; ++i)
    {
        argList += argv[i];
        argList += " ";
    }
    LogInfo("Command line arguments: %s", argList.c_str());

    return EXIT_SUCCESS;
}

i32 Engine::Init()
{
    LogInfo("Engine::Init");
    m_pWindow = Window::Create();
    Renderer::Create();
    m_pCurrentScene = new Scene();

    m_pCurrentScene->AddUpdateSystem(new SCamera());


    auto KeyCallback = [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
	{
		InputHandler::GetInstance().HandleKeyInput(window, key, scancode, action, mods);
	};
    glfwSetKeyCallback(m_pWindow->GetWindow(), KeyCallback);

    auto MouseCallback = [](GLFWwindow* window, f64 xpos, f64 ypos)
    {
        InputHandler::GetInstance().HandleMousePosChange(window, xpos, ypos);
    };
    glfwSetCursorPosCallback(m_pWindow->GetWindow(), MouseCallback);

    //glfwSetInputMode(m_pWindow->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return EXIT_SUCCESS;
}

void Engine::UpdateRender()
{
    m_CurrentFrameTime = glfwGetTime();
    m_DeltaTime = m_CurrentFrameTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentFrameTime;

    m_pCurrentScene->Update(m_DeltaTime);
    m_pRenderer->Draw();
}

void Render()
{
    Engine::Instance().UpdateRender();
}

i32 Engine::MainLoop()
{
    LogInfo("Engine::MainLoop");

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(Render, 0, false);
#else
    while(m_pWindow->PollEvents())
    {
		m_CurrentFrameTime = glfwGetTime();
		m_DeltaTime = m_CurrentFrameTime - m_LastFrameTime;
		m_LastFrameTime = m_CurrentFrameTime;

        m_pCurrentScene->Update(m_DeltaTime);
        m_pRenderer->Draw();
    }
#endif
    return EXIT_SUCCESS;
}

i32 Engine::Shutdown()
{
    delete m_pRenderer;
    delete m_pWindow;
    delete m_pCurrentScene;

    return EXIT_SUCCESS;
}

Window* Engine::GetWindow() const
{
    return m_pWindow;
}

Renderer* Engine::GetRenderer() const
{
    return m_pRenderer;
}

Scene* Engine::GetCurrentScene() const
{
    return m_pCurrentScene;
}

entt::entity Engine::CreateEntity()
{
    return Instance().GetCurrentScene()->m_Registry.create();
}

wgpu::Device& Engine::GetDevice()
{
    return Instance().GetRenderer()->GetDevice();
}

wgpu::Queue& Engine::GetQueue()
{
    return Instance().GetRenderer()->GetQueue();
}

GLFWwindow* Engine::GetGLFWWindow()
{
    return m_pWindow->GetWindow();
}

void Engine::SetRenderer(Renderer *renderer)
{
    m_pRenderer = renderer;
}


}