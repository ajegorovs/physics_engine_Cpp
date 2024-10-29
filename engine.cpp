#include "engine.h"
#include "debug2.h"
#include "glfw_support.h"
#include "shapes.h"
#include "physics.h"
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <chrono>
#include <glm/ext/scalar_constants.hpp>
#include <random>  
#include "MathsGL.h"

Engine::Engine() :
    glfw_s(),
    dvc(&instance, &surface, &device),
    swp(&surface, &device, &dvc.physicalDevice, &dvc.msaaSamples),
    sync(&device),
    gfx(&device, &dvc.msaaSamples),
    cmpt(&device),
    dscr(&device),
    cmd(&device, &dvc.physicalDevice, &surface),
    bfr(&device, &dvc.physicalDevice, &cmd.commandPool, &graphicsQueue) 
{
}



void Engine::run() 
{
    //std::vector<glm::vec3> points = { {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.5f, 1.5f, 1.5f} };
    //glm::vec3 v(2.f, 2.f, 2.f);
    //MathGL::calculateSquaredDistanceUpperTriangleMatrix(points);
    center = glm::vec3(0.0f, 0.0f, 0.0f);
    center2 = glm::vec3(0.0f, 0.0f, 0.0f);
    center_mass = glm::float32(20.0f);
    center_mass2 = glm::float32(0.0f);
    grav_const = glm::float32(1.0f);
    glm::vec3 reference_axis = glm::vec3(0, 0, 1);
        
    //std::cout << VkApplicationInfo::apiVersion() << std::endl;
    constexpr float pi = glm::pi<float>();
    std::vector<std::unique_ptr<geometric_shape>> scene;
    //scene.push_back(std::make_unique<Plane>(glm::vec3(3, 3, 0)      , glm::vec3(0, 0, -10.5f)    , glm::vec2(0, 0)));
    scene.push_back(std::make_unique<Prism>(glm::vec3(1, 0.5, 0.5)  , glm::vec3(0, 0, 1.5)  , glm::vec2(glm::radians(45.), glm::radians(30.))));
    //scene.push_back(std::make_unique<Cube>( 0.5                     , glm::vec3(2, 1, 0.5) , glm::vec2(0, 0)));

    // ===== UI ===== //
    glfw_s.initWindow();                                                    // window
    // === VULKAN === //
    createInstance();                                                       // instance
    dbg.setupDebugMessenger(instance, device);
    glfw_s.createSurface(instance, &surface);
	dvc.pickPhysicalDevice();                                               // physicalDevice
    dvc.createLogicalDevice(validationLayers, &graphicsQueue, &presentQueue, &computeQueue); // device, graphicsQueue and presentQueue
    vkCmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT)vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopologyEXT");
    swp.createSwapChain(glfw_s.window);
    swp.createImageViews();
    swp.createRenderPass(dvc.findDepthFormat());                            // renderPass

    dscr.createDSL_uniformMVP();
    dscr.createDSL_multi_MPV_TS_TRN();
    dscr.createDSL_1UC_2SC();

    gfx.createGraphicsPipeline_storageVertices(&dscr.descriptorSetLayout_multi_MPV_TS_TRN, &swp.renderPass);                           // pipelineLayout, graphicsPipeline
    gfx.createGraphicsPipeline_storageParticles(&dscr.descriptorSetLayout_uniformMVP, &swp.renderPass);                          // pipelineLayout, graphicsPipeline
    cmpt.createComputePipeline_particle(&dscr.descriptorSetLayout_1UC_2SC);                          // pipelineLayout, graphicsPipeline

    cmd.createCommandPool();                                                // commandPool
    swp.createColorResources();                                             // colorImageView
    swp.createDepthResources(dvc.findDepthFormat());                        // depthImageView
    swp.createFramebuffers();                                // swapChainFramebuffers
    swp.createTextureImage(cmd, graphicsQueue);                             //
    swp.createTextureImageView();
    swp.createTextureSampler();                                             //

    bfr.processScene(scene);                                                // yeet vertices, indices and IDs into pre-buffers

    bfr.createVertexBuffer();
    bfr.createIndexBuffer();

    bfr.createBuffer_uniformMVP();
    bfr.createBuffer_storageTransformations();
    bfr.createBuffer_storageParticles(          center, center_mass, grav_const, reference_axis);                                    // will act as vertex buffer for particles
    bfr.createBuffer_storageComputeParticles(   center, center_mass, grav_const, reference_axis);                                    // will act as vertex buffer for particles
    bfr.createBuffer_uniformDeltaTime();

    dscr.createDescriptorPool();

    dscr.createDS_multi_MPV_TS_TRN(
        bfr.buffer_uniformMVP,  bfr.buffer_storageTransformations,
        swp.textureImageView,   swp.textureSampler
    );
    dscr.createDS_uniformMVP(bfr.buffer_uniformMVP);
    dscr.createDS_1UC_2SC_1Time_2ParticleParams(
        bfr.buffer_uniformDeltaTime, 
        bfr.buffer_storageComputeParticles
    );

    cmd.createCommandBuffers();
    cmd.createComputeCommandBuffers();
    sync.createSyncObjects();
    bool firstFrameCompleted = false; // second frame has anamolous frame time.
    double currentTime = 0.0f;
    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(glfw_s.window)) {
        glfwPollEvents();
        drawFrame();
        currentTime = glfwGetTime();

        if (firstFrameCompleted) {
            lastFrameTime = (currentTime - lastTime);// *1000.0;
        }
        else {
            lastFrameTime = 0.0f;
            firstFrameCompleted = true; 
        }

        lastTime = currentTime;

        glfw_s.setWindowTitleWithFPS(lastFrameTime);
        
        //std::cout << lastFrameTime << std::endl;
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

//void Engine::updateBufferMapped_storageParticles() {
//
//    if (1 == 0) {
//
//        std::default_random_engine rndEngine((unsigned)time(nullptr));
//        std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);
//
//        // init
//        float id = 0;
//        std::vector<point3D> particles(PARTICLE_COUNT);
//        for (auto& particle : particles) {
//            particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
//            particle.position = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine));
//            particle.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
//            particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
//            particle.mass = glm::float32(1.0);
//            particle.damping = glm::float32(id);
//
//            id += 0.1;
//        }
//
//        VkDeviceSize bufferSize = sizeof(point3D) * PARTICLE_COUNT;
//
//        memcpy(bfr.bufferMapped_storageParticles[currentFrame], particles.data(), (size_t)bufferSize);
//
//    };
//
//    point3D* particles = reinterpret_cast<point3D*>(bfr.bufferMapped_storageParticles[currentFrame]);
//
//    static auto startTime2 = std::chrono::high_resolution_clock::now();
//    auto currentTime2 = std::chrono::high_resolution_clock::now();
//    float time2 = std::chrono::duration<float, std::chrono::seconds::period>(currentTime2 - startTime2).count();
//
//    float scaleFactor = 1.0f + 0.01f * std::cos(glm::radians(90.0f)* time2) ;
//
//    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
//
//    for (size_t i = 0; i < PARTICLE_COUNT; i++) {
//        //particles[i].position = (transform * glm::vec4(particles[i].position, 1.0f)).xyz;
//        particles[i].position += particles[i].velocity * time2;
//    }
//}

void Engine::updateBufferMapped_uniformDeltaTime(uint32_t currentImage) {
    StructDeltaTime ubo{};
    ubo.deltaTime = lastFrameTime * 12.0f;

    memcpy(bfr.bufferMapped_uniformDeltaTime[currentImage], &ubo, sizeof(ubo));
}

void Engine::updateBufferMapped_storageParticles(uint32_t currentFrame) {


    int previousFrame = (currentFrame - 1) % MAX_FRAMES_IN_FLIGHT;
    point3D* particles_prev = reinterpret_cast<point3D*>(bfr.bufferMapped_storageParticles[previousFrame]);
    point3D* particles_crnt = reinterpret_cast<point3D*>(bfr.bufferMapped_storageParticles[currentFrame]);

    //float length0 = glm::length(particles_prev[0].position);

    //std::cout << "CF: " << currentFrame << " ; " << "PF: " << previousFrame << " ; " << lastFrameTime*1000 << "(ms), L(i-1) = " << length0;// << " " << particles_crnt[0].velocity << " " << particles_prev[0].velocity
    for (size_t i = 0; i < PARTICLE_COUNT; i++) {
        // Verlet integration https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet
        // Update pos using old data r_{t}
        particles_crnt[i].position = particles_prev[i].position +
            particles_prev[i].velocity * lastFrameTime +
            0.5f * particles_prev[i].acceleration * lastFrameTime * lastFrameTime;

        // Calculate new accel a(t+dt) = F_{t+dt}/m

        glm::vec3 dir = center - particles_crnt[i].position;
        glm::float32 distSquared = glm::dot(dir, dir);
        glm::vec3 dir_normalized = glm::normalize(dir);

        /*glm::vec3 dir2 = center2 - particles_crnt[i].position;
        glm::float32 distSquared2 = glm::dot(dir2, dir2);
        glm::vec3 dir_normalized2 = glm::normalize(dir2);*/

        particles_crnt[i].acceleration = grav_const * (center_mass / distSquared * dir_normalized);// + center_mass2 / distSquared2 * dir_normalized2);

        // Update velocity using average acceleration v_{t+dt}
        particles_crnt[i].velocity = particles_prev[i].velocity +
            0.5f * (particles_prev[i].acceleration + particles_crnt[i].acceleration) * lastFrameTime;


        if (particles_crnt[i].position.x < -5.0f || particles_crnt[i].position.x > 5.0f){
            particles_crnt[i].velocity.x *= -1;
        }
        if (particles_crnt[i].position.y < -5.0f || particles_crnt[i].position.y > 5.0f) {
            particles_crnt[i].velocity.y *= -1;
        }
        if (particles_crnt[i].position.z < -5.0f || particles_crnt[i].position.z > 5.0f) {
            particles_crnt[i].velocity.z *= -1;
        }

    }
    //float length1 = glm::length(particles_crnt[1].position);
    //std::cout <<"; L(i) = "<< length1 << std::endl;
}


void Engine::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    StructMVP ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), 0.0f*0.5f * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swp.swapChainExtent.width / (float)swp.swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(bfr.bufferMapped_uniformMVP[currentImage], &ubo, sizeof(ubo));
}

void Engine::updateStorageBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    StructObjectTransformations sbo{};
    for (size_t i = 0; i < sbo.model.size(); i++) {
        float omega = static_cast<float>(pow(-1, i) * 2 * i) * glm::radians(90.0f);
        glm::vec3 axis_rot(0.f, 0.f, 1.f);

        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), omega * time, axis_rot);
        glm::vec3 axis_trans(0.f, 0.f, 0.1 * glm::cos( 5. * omega * time));

        sbo.model[i] = glm::translate(transform, axis_trans);

    }
    memcpy(bfr.bufferMapped_storageTransformtions[currentImage], &sbo, sizeof(sbo));
}

void Engine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swp.renderPass;
    renderPassInfo.framebuffer = swp.swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swp.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline_particles);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout_particles, 0, 1, &dscr.descriptorSets_uniformMVP[currentFrame], 0, nullptr);

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
    
    // ================ CURRENT PROBLEM AND SETTING ===================
    // initializing vertex data on device-local memory (storage & vertex mem)
    // without modifying binding this mem to vertex data of a gfx pipeline.

    // draw compute on gfx particle pipeline

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &bfr.buffer_storageComputeParticles[currentFrame], offsets);

    vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

    vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);

    //// draw cpu particles
    //VkBuffer vertexBuffers2[] = { bfr.buffer_storageParticles[currentFrame]};
    //vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers2, offsets);

    //vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);
    // draw polygon geom
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline);

    VkBuffer vertexBuffers3[] = { bfr.vertexBuffer };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers3, offsets);

    vkCmdBindIndexBuffer(commandBuffer, bfr.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout, 0, 1, &dscr.descriptorSets_multi_MPV_TS_TRN[currentFrame], 0, nullptr);

    vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkCmdSetLineWidth(commandBuffer, 2.0f);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(bfr.indices.size()), 1, 0, 0, 0);


    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Engine::recordComputeCommandBuffer(VkCommandBuffer commandBuffer) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePipeline);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePipelineLayout, 0, 1, &dscr.descriptorSets_1UC_2SC[currentFrame], 0, nullptr);

    vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
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
    swp.createFramebuffers();
}

void Engine::drawFrame() {

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Compute submission        
    vkWaitForFences(device, 1, &sync.computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    updateBufferMapped_uniformDeltaTime(currentFrame);

    vkResetFences(device, 1, &sync.computeInFlightFences[currentFrame]);

    vkResetCommandBuffer(cmd.computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordComputeCommandBuffer(cmd.computeCommandBuffers[currentFrame]);

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd.computeCommandBuffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &sync.computeFinishedSemaphores[currentFrame];
    VkResult resultCompute = vkQueueSubmit(computeQueue, 1, &submitInfo, sync.computeInFlightFences[currentFrame]);
    if (resultCompute != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute command buffer!");
    };

    // Graphics submission
    vkWaitForFences(device, 1, &sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //updateBufferMapped_storageParticles(currentFrame);

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
    

    VkSemaphore waitSemaphores[] = { 
        sync.computeFinishedSemaphores[currentFrame],
        sync.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { 
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

   
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
    gfx.cleanup();
    cmpt.cleanup();
    swp.cleanupRenderPass();
    bfr.clearBuffers1();
    dscr.cleanupDPool();
    swp.cleanupRest();
    dscr.cleanupDSL();
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

