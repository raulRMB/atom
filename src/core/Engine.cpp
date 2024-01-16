//
// Created by Raul Romero on 2024-01-13.
//

#include <string>
#include "Engine.h"
#include "Renderer.h"
#include "Window.h"
#include "Logger.h"

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
    m_pWindow = Window::Create();
    m_pRenderer = Renderer::Create();

    return EXIT_SUCCESS;
}

i32 Engine::MainLoop()
{
    while(m_pWindow->PollEvents())
    {
        m_pRenderer->Draw();
    }

    return EXIT_SUCCESS;
}

i32 Engine::Shutdown()
{
    delete m_pRenderer;
    delete m_pWindow;

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

GLFWwindow* Engine::GetGLFWWindow()
{
    return m_pWindow->GetWindow();
}


}