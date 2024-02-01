//
// Created by Raul Romero on 2024-01-13.
//

#include "App.h"
#include <iostream>
#include "Window.h"

namespace atom
{

App::App()
{
}

App::~App()
{
}

i32 App::Run(int argc, char** argv)
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

App& App::Instance()
{
    static App instance;
    return instance;
}

void App::Start()
{
    m_bRunning = true;
}

}