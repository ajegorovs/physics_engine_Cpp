#pragma once
#include "partUno.h"
#include "partDos.h"

class VApp {
public:
    PartUno p1;
    PartDos p2;
    void run();
    bool framebufferResized = false; 
    GLFWwindow* window;

private:
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
