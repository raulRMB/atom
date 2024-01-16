//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_WINDOW_H
#define ATOM_WINDOW_H

struct GLFWwindow;

namespace atom
{

class Window
{
private:
    friend class Engine;
private:
    Window();
    ~Window();
private:
    static Window* Create();

    GLFWwindow* m_pWindow;
    bool ShouldClose();
    [[nodiscard]] GLFWwindow* GetWindow() const;

    bool PollEvents();
};

}

#endif //ATOM_WINDOW_H
