//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_APP_H
#define ATOM_APP_H

#include "../Defines.h"
#include "Engine.h"

namespace atom
{

class App
{
public:
    App();
    ~App();
    static i32 Run(int argc, char** argv);
private:
    static App& Instance();
    void Start();
private:
    class Engine m_Engine;
    bool m_bRunning;
};

}

#endif //ATOM_APP_H
