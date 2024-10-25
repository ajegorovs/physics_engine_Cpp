#include "engine.h"
#include "debug2.h"
#include "glfw_support.h"
#include "shapes.h"
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <chrono>
#include <glm/ext/scalar_constants.hpp>



Engine::Engine() :
    glfw_s(),
    dvc(&instance, &surface, &device),
    swp(&surface, &device, &dvc.physicalDevice, &dvc.msaaSamples),
    sync(&device),
    rndr(&device, &dvc.msaaSamples),
    cmd(&device, &dvc.physicalDevice, &surface),
    bfr(&device, &dvc.physicalDevice, &cmd.commandPool, &graphicsQueue) 
{
}



void Engine::run() 
{
    //std::cout << VkApplicationInfo::apiVersion() << std::endl;
    constexpr float pi = glm::pi<float>();
    std::vector<std::unique_ptr<geometric_shape>> scene;
    scene.push_back(std::make_unique<Plane>(glm::vec3(3, 3, 0)      , glm::vec3(0, 0, 0)    , glm::vec2(0, 0)));
    scene.push_back(std::make_unique<Prism>(glm::vec3(1, 0.5, 0.5)  , glm::vec3(0, 0, 1.5)  , glm::vec2(glm::radians(45.), glm::radians(30.))));
    scene.push_back(std::make_unique<Cube>( 0.5                     , glm::vec3(2, 1, 0.5) , glm::vec2(0, 0)));

    // ===== UI ===== //
    glfw_s.initWindow();                                                    // window
    // === VULKAN === //
    createInstance();                                                       // instance
    dbg.setupDebugMessenger(instance, device);
    glfw_s.createSurface(instance, &surface);
	dvc.pickPhysicalDevice();                                               // physicalDevice
    dvc.createLogicalDevice(validationLayers, &graphicsQueue, &presentQueue); // device, graphicsQueue and presentQueue
    vkCmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT)vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopologyEXT");
    swp.createSwapChain(glfw_s.window);
    swp.createImageViews();
    rndr.createRenderPass(swp.swapChainImageFormat, dvc.findDepthFormat()); // renderPass
    rndr.createDescriptorSetLayout();                                       // descriptorSetLayout
    //rndr.createComputeDescriptorSetLayout();

    rndr.createGraphicsPipeline();                                          // pipelineLayout, graphicsPipeline
    //rndr.createParticleGraphicsPipeline();

    //rndr.createComputePipeline();
    cmd.createCommandPool();                                                // commandPool
    swp.createColorResources();                                             // colorImageView
    swp.createDepthResources(dvc.findDepthFormat());                        // depthImageView
    swp.createFramebuffers(rndr.renderPass);                                // swapChainFramebuffers
    swp.createTextureImage(cmd, graphicsQueue);                             //
    swp.createTextureImageView();
    swp.createTextureSampler();                                             //
    bfr.processScene(scene);
    bfr.createVertexBuffer();
    bfr.createIndexBuffer();
    bfr.createUniformBuffers();
    //bfr.createParticleUniformBuffers();
    bfr.createStorageBuffers();
    //bfr.createShaderStorageBuffers();
    swp.createDescriptorPool();                                             //descriptorPool
    swp.createDescriptorSets(rndr.descriptorSetLayout, bfr.uniformBuffers, bfr.storageBuffers);
    //swp.createComputeDescriptorSets(rndr.computeDescriptorSetLayout, bfr.particleUniformBuffers, bfr.shaderStorageBuffers);
    cmd.createCommandBuffers();
    //cmd.createComputeCommandBuffers();
    sync.createSyncObjects();

    while (!glfwWindowShouldClose(glfw_s.window)) {
        glfwPollEvents();
        drawFrame();
        //double currentTime = glfwGetTime();
        //lastFrameTime = (currentTime - lastTime) * 1000.0;
        //lastTime = currentTime;
        //std::cout << num_frames << std::endl;
    }
    vkDeviceWaitIdle(device);
    cleanup();
};


//Vulkan init
void Engine::createInstance() 
{
    
    if (enableValidationLayers && !Debug2::checkValidationLayerSupport(validationLayers)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GLFW_support::getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        Debug2::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void Engine::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swp.swapChainExtent.width / (float)swp.swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(bfr.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

//void Engine::updateParticleUniformBuffer(uint32_t currentImage) {
//    ParticleUniformBufferObject pubo{};
//    pubo.deltaTime = lastFrameTime * 2.0f;
//
//    memcpy(bfr.particleUniformBuffersMapped[currentImage], &pubo, sizeof(pubo));
//}

void Engine::updateStorageBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    StorageBufferObject sbo{};
    for (size_t i = 0; i < sbo.model.size(); i++) {
        float omega = static_cast<float>(pow(-1, i) * 2 * i) * glm::radians(90.0f);
        glm::vec3 axis_rot(0.f, 0.f, 1.f);

        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), omega * time, axis_rot);
        glm::vec3 axis_trans(0.f, 0.f, 0.1 * glm::cos( 5. * omega * time));

        sbo.model[i] = glm::translate(transform, axis_trans);

    }
    memcpy(bfr.storageBuffersMapped[currentImage], &sbo, sizeof(sbo));
}

void Engine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = rndr.renderPass;
    renderPassInfo.framebuffer = swp.swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swp.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rndr.graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swp.swapChainExtent.width;
    viewport.height = (float)swp.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swp.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offsets[] = { 0 };
    VkBuffer vertexBuffers[] = { bfr.vertexBuffer };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, bfr.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rndr.pipelineLayout, 0, 1, &swp.descriptorSets[currentFrame], 0, nullptr);

    vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    vkCmdSetLineWidth(commandBuffer, 10.0f);
    
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(bfr.indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


//void Engine::recordComputeCommandBuffer(VkCommandBuffer commandBuffer)
//{
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
//        throw std::runtime_error("failed to begin recording compute command buffer!");
//    }
//
//    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, rndr.computePipeline);
//
//    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, rndr.computePipelineLayout, 0, 1, &swp.computeDescriptorSets[currentFrame], 0, nullptr);
//
//    vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);
//
//    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//        throw std::runtime_error("failed to record compute command buffer!");
//    }
//}

void Engine::recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(glfw_s.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(glfw_s.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    swp.cleanupSwapChain();

    swp.createSwapChain(glfw_s.window);
    swp.createImageViews();
    swp.createColorResources();// colorImageView
    swp.createDepthResources(dvc.findDepthFormat());   // depthImageView
    swp.createFramebuffers(rndr.renderPass);
}

void Engine::drawFrame() {

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Compute submission        
    //vkWaitForFences(device, 1, &sync.computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //updateParticleUniformBuffer(currentFrame);
    //
    //vkResetFences(device, 1, &sync.computeInFlightFences[currentFrame]);

    //vkResetCommandBuffer(cmd.computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    //recordComputeCommandBuffer(cmd.computeCommandBuffers[currentFrame]);

    //submitInfo.commandBufferCount = 1;
    //submitInfo.pCommandBuffers = &cmd.computeCommandBuffers[currentFrame];
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = &sync.computeFinishedSemaphores[currentFrame];
    //VkResult res = vkQueueSubmit(computeQueue, 1, &submitInfo, sync.computeInFlightFences[currentFrame]);
    //if (res != VK_SUCCESS) {
    //    throw std::runtime_error("failed to submit compute command buffer!");
    //};

    // Graphics submission
    vkWaitForFences(device, 1, &sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    updateStorageBuffer(currentFrame);
    updateUniformBuffer(currentFrame);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swp.swapChain, UINT64_MAX, sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &sync.inFlightFences[currentFrame]);

    vkResetCommandBuffer(cmd.commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(cmd.commandBuffers[currentFrame], imageIndex);
    

    /*VkSemaphore waitSemaphores[] = { sync.computeFinishedSemaphores[currentFrame], sync.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };*/
    VkSemaphore waitSemaphores[] = {sync.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(std::size(waitSemaphores));
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd.commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { sync.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(std::size(signalSemaphores));
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, sync.inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swp.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || glfw_s.framebufferResized) {
        glfw_s.framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::cleanup() {
    std::cout << std::string(40, '-') <<" CLEANUP " << std::string(40, '-') << std::endl;
    swp.cleanupSwapChain();
    rndr.cleanup();
    bfr.clearBuffers1();
    swp.cleanupRest();
    bfr.clearBuffers2();
    sync.cleanup();
    vkDestroyCommandPool(device, cmd.commandPool, nullptr);
    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        dbg.DestroyDebugUtilsMessengerEXT(instance, dbg.debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(glfw_s.window);

    glfwTerminate();
};

