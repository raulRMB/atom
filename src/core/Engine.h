//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_ENGINE_H
#define ATOM_ENGINE_H

#include "../Defines.h"

struct GLFWwindow;

namespace atom
{

class Engine
{
public:
    friend class Application;

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

public:
    Window* GetWindow() const;
    GLFWwindow* GetGLFWWindow();
    Renderer* GetRenderer() const;
};

}

#endif //ATOM_ENGINE_H
