//
// Created by Raul Romero on 2024-01-13.
//

#include "Window.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

namespace atom
{

Window::Window() : m_pWindow(nullptr)
{
    if (!glfwInit())
    {
        LogError("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_pWindow = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);

    if (!m_pWindow)
    {
        LogError("Failed to create window");
        return;
    }
}

Window::~Window()
{
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

Window *Window::Create()
{
    return new Window();
}

GLFWwindow *Window::GetWindow() const
{
    return m_pWindow;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_pWindow);
}

bool Window::PollEvents()
{
    glfwPollEvents();
    return !ShouldClose();
}

}