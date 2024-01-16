//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_APPLICATION_H
#define ATOM_APPLICATION_H

#include "../Defines.h"
#include "Engine.h"

namespace atom
{

class Application
{
public:
    Application();
    ~Application();
    static i32 Run(int argc, char** argv);
private:
    static Application& Instance();
    void Start();
private:
    class Engine m_Engine;
    bool m_bRunning;
};

}

#endif //ATOM_APPLICATION_H
