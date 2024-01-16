//
// Created by Raul Romero on 2024-01-13.
//

#include "Application.h"
#include <iostream>
#include "Window.h"

namespace atom
{

Application::Application()
{
}

Application::~Application()
{
}

i32 Application::Run(int argc, char** argv)
{
    if(Instance().m_bRunning)
    {
        std::cout << "Application is already running." << std::endl;
        return EXIT_FAILURE;
    }

    Instance().Start();

    Engine::Instance().Run(argc, argv);

    return EXIT_SUCCESS;
}

Application& Application::Instance()
{
    static Application instance;
    return instance;
}

void Application::Start()
{
    m_bRunning = true;
}

}