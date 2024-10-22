#include "UI.h"

void UI::connect(std::shared_ptr<DebugEnvironment> env) {
    deb = env; 
};

void UI::initWindow() {
    timer.line_init("initWindow");
    //std::cout << "initWindow() initializing ... " << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    base->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(base->window, this);
    glfwSetFramebufferSizeCallback(base->window, framebufferResizeCallback);
    timer.line_end("initWindow");
}
void UI::createInstance() {
    timer.line_init("createInstance");
    if (enableValidationLayers && !(deb->checkValidationLayerSupport())) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(base->validationLayers.size());
        createInfo.ppEnabledLayerNames = base->validationLayers.data();

        deb->populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &(base->instance)) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    timer.line_end("createInstance");

}


void UI::createSurface() {
    timer.line_init("createSurface");
    auto a = glfwCreateWindowSurface(base->instance, base->window, nullptr, &(base->surface));
    if (a != VK_SUCCESS) {
        std::cout << a << "\n";
        throw std::runtime_error("failed to create window surface!");
    }
    timer.line_end("createSurface");

}

void UI::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<UI*>(glfwGetWindowUserPointer(window));
    app->base->framebufferResized = true; // lets store it in baseEnv, so i dont have to connect.
};

std::vector<const char*> UI::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}