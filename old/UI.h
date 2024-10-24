#pragma once
//#include "config.h"
//#include "DebugEnvironment.h"
//#include "BaseEnvironment.h"

#ifndef UI_H
#define UI_H

#include "Timer.h"
#include "BaseEnvironment.h"
#include "DebugEnvironment.h"

#include "config.h"


class UI {
private:
    std::shared_ptr<BaseEnvironment> base; // Shared pointer to the base environment
    std::shared_ptr<DebugEnvironment> deb;
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    bool framebufferResized = false;
    std::vector<const char*> getRequiredExtensions();
    Timer timer;
public:
    UI(std::shared_ptr<BaseEnvironment> env) : base(env) {}
    void connect(std::shared_ptr<DebugEnvironment> env);
    void initWindow();
    void createInstance();
    void createSurface();
};

#endif