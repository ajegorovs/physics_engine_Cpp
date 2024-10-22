#pragma once
#include "config.h"

#include "BaseEnvironment.h"
#include "DebugEnvironment.h"
#include "UI.h"
#include "Device.h"
#include "Image.h"
#include "Resources.h"
#include "Cleanup.h"


int main() {
    std::shared_ptr<BaseEnvironment> pBase = std::make_shared<BaseEnvironment>();
    std::shared_ptr<DebugEnvironment> pDebug = std::make_shared<DebugEnvironment>(pBase);
    std::shared_ptr<UI> pUI = std::make_shared<UI>(pBase);
    std::shared_ptr<Device> pDvc = std::make_shared<Device>(pBase);
    std::shared_ptr<Image> pImg = std::make_shared<Image>(pBase);
    std::shared_ptr<Resources> pRes = std::make_shared<Resources>(pBase);
    std::shared_ptr<Cleanup> pCln = std::make_shared<Cleanup>(pBase);
    pUI ->connect(pDebug);
    pImg->connect(pDvc);
    pRes->connect(pDvc, pImg);
    pCln->connect(pDvc, pImg, pRes);

    try {
        float pi = glm::pi<float>();
        std::vector<std::unique_ptr<geometric_shape>> scene;
        scene.push_back(std::make_unique<Prism>(glm::vec3(1, 0.5, 0.5)  , glm::vec3(0, 0, 0.5)  , glm::vec2(glm::radians(45.), glm::radians(0.))));
        scene.push_back(std::make_unique<Cube>(0.5                  , glm::vec3(0, 0, 1)    , glm::vec2(0, 0)));
        scene.push_back(std::make_unique<Plane>(glm::vec3(3, 3, 0)  , glm::vec3(0, 0, 0)    , glm::vec2(0, 0)));

        pUI->initWindow();
        pUI->createInstance();
        pDebug->setupDebugMessenger();
        pUI->createSurface();
        pDvc->pickPhysicalDevice();
        pDvc->createLogicalDevice();
        pImg->createSwapChain();
        pImg->createImageViews();
        pImg->createRenderPass();
        pRes->createDescriptorSetLayout();
        pRes->createGraphicsPipeline();
        pRes->createCommandPool();
        pRes->createColorResources();
        pRes->createDepthResources();
        pRes->createFramebuffers();
        pRes->createTextureImage();
        pRes->createTextureImageView();
        pRes->createTextureSampler();
        pRes->processScene(scene);
        //pRes->loadModel();
        pRes->createVertexBuffer();
        pRes->createIndexBuffer();
        pRes->createUniformBuffers();
        pRes->createDescriptorPool();
        pRes->createDescriptorSets();
        pRes->createCommandBuffers();
        pRes->createSyncObjects();
        
        //processScene(scene);

        while (!glfwWindowShouldClose(pBase->window)) {
            glfwPollEvents();
            pRes->drawFrame();
            //std::cout << "\b" << pUI->print();
        }
        pCln->cleanup();
        //std::cout << pBase.get();

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}