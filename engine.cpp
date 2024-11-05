#include "engine.h"
#include "debug2.h"
#include "glfw_support.h"
#include "shapes.h"
#include "physics.h"
#include "misc.h"
#include "enable_stuff.h"
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <chrono>
#include <glm/ext/scalar_constants.hpp>
#include <random>  
#include "MathsGL.h"
#include "lbvh.h"

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
    cnt = 0;

    //const std::vector<glm::vec3> poits2d = Misc::seedUniformPoints2D(50);

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

    

    // ================ PIPELINES =====================
      
    gfx.createGraphicsPipeline_storageLines(     
        &dscr.descriptorSetLayout_uniformMVP,
        &swp.renderPass
    );
    gfx.createGraphicsPipeline_storageParticles(    
        &dscr.descriptorSetLayout_uniformMVP, 
        &swp.renderPass
    );   
 
    
    cmd.createCommandPool();                                                // commandPool
    swp.createColorResources();                                             // colorImageView
    swp.createDepthResources(dvc.findDepthFormat());                        // depthImageView
    swp.createFramebuffers();                                               // swapChainFramebuffers
    swp.createTextureImage(cmd, graphicsQueue);                             //
    swp.createTextureImageView();
    swp.createTextureSampler();                                             //
 
    //bfr.processGrid();
    
    bfr.createBuffer_line();

    bfr.createBuffer_uniformMVP();

    dscr.createDescriptorPool();

    dscr.createDS_uniformMVP(bfr.buffer_uniformMVP);

    cmd.createCommandBuffers();

    sync.createSyncObjects();

    //================== LVBH ==================
    if (ENABLE_LVBH) {

        dscr.createDSL_lbvh();
        //dscr.createDSL_lbvh_transform();
        cmpt.createComputePipeline_lbvh(dscr.descriptorSetLayout_lbvh);
        //const std::vector<glm::vec3> points3d = Misc::seedUniformPoints2D(NUM_ELEMENTS);
        //const std::vector<glm::vec3> points3d = Misc::seedUniformGridPoints3D(NUM_ELEMENTS);
        const std::vector<glm::vec3> points3d = Misc::seedUniformSpherePoints3D(NUM_ELEMENTS);
        //std::vector<glm::vec3> points3d = Misc::sortByMorton(points3d_0);
        bfr.createBuffer_lbvh_points(points3d, glm::vec3(0.0f, 0.0f, 1.0f));
        bfr.createBuffer_lbvh_elementsBuffer(points3d);
        bfr.createBuffer_lbvh_mortonCode();
        bfr.createBuffer_lbvh_mortonCodePingPong();
        bfr.createBuffer_lbvh_LBVH();
        bfr.createBuffer_lbvh_LBVHConstructionInfo();
        bfr.createBuffer_lbvh_LBVH_hist_vis();

        dscr.createDS_lbvh(
            bfr.buffer_lbvh_elements,
            bfr.buffer_lbvh_mortonCode,
            bfr.buffer_lbvh_mortonCodePingPong,
            bfr.buffer_lbvh_LBVH,
            bfr.buffer_lbvh_LBVHConstructionInfo,
            bfr.buffer_lbvh_particles
        );
        //dscr.createDS_lbvh_transform(bfr.buffer_lbvh_particles, bfr.buffer_lbvh_elements);
        cmd.createLBVHComputeCommandBuffer();

    }
     
    if (ENABLE_POLY) {
        dscr.createDSL_multi_MPV_TS_TRN();
        gfx.createGraphicsPipeline_storageVertices(     // pipelineLayout, graphicsPipeline
            &dscr.descriptorSetLayout_multi_MPV_TS_TRN,
            &swp.renderPass
        );
        std::vector<std::unique_ptr<geometric_shape>> scene;
        //scene.push_back(std::make_unique<Plane>(glm::vec3(3, 3, 0)      , glm::vec3(0, 0, -10.5f)    , glm::vec2(0, 0)));
        scene.push_back(std::make_unique<Prism>(glm::vec3(1, 0.5, 0.5), glm::vec3(0, 0, 1.5), glm::vec2(glm::radians(45.), glm::radians(30.))));
        //scene.push_back(std::make_unique<Cube>( 0.5                     , glm::vec3(2, 1, 0.5) , glm::vec2(0, 0)));

        bfr.processScene(scene);

        bfr.createVertexBuffer();
        bfr.createIndexBuffer();

        bfr.createBuffer_storageTransformations();

        dscr.createDS_multi_MPV_TS_TRN(
            bfr.buffer_uniformMVP, bfr.buffer_storageTransformations,
            swp.textureImageView, swp.textureSampler
        );

    }
    if (ENABLE_PHYSICS) {

        masses.push_back(20.0f);
        masses.push_back(4.0f);
        radiuses.push_back(1.0f);
        radiuses.push_back(1.0f);
        densities.push_back(1.0f);
        densities.push_back(1.0f);
        positions.push_back(glm::vec3(0.0f, -0.5f, 0.0f));
        positions.push_back(glm::vec3(0.0f, -0.2f, 0.0f));

        dscr.createDSL_1UC_4SC();

        cmpt.createComputePipeline_particle(&dscr.descriptorSetLayout_1UC_4SC);

        bfr.createBuffer_uniformDeltaTime();
        bfr.createBuffer_physics_particles_constants();
        bfr.createBuffer_physics_attractors(
            masses,
            radiuses,
            densities,
            positions
        );
        bfr.createBuffer_physics_particles_compute();

        dscr.createDS_physics_compute(      // to descriptorSets_1UC_4SC
            bfr.buffer_uniformDeltaTime,    // deltaTime
            bfr.buffer_physics_constants,   // constants
            bfr.buffer_physics_attractors,  // attractors
            bfr.buffer_physics_particles    // particle old/new
        );

        cmd.createComputeCommandBuffers();
    }

    bool firstFramebufferCompleted = false; // second frame has anamolous frame time.
    uint32_t starter_frame_counter = 0;
    double currentTime = 0.0f;
    lastTime = glfwGetTime();
    recalculateLBVH = true;
    while (!glfwWindowShouldClose(glfw_s.window)) {
        glfwPollEvents();
        drawFrame();
        currentTime = static_cast<double>(glfwGetTime());

        // physics uses frame times and its busted for 1st framebuffer
        // set frame times to 0 so physics is not advanced
        // i also use it to detect initialization frames in compute shader.
        if (firstFramebufferCompleted) {
            lastFrameTime = (currentTime - lastTime);
        }
        else {
            lastFrameTime = 0.0;
            starter_frame_counter++;
        }
        if (!firstFramebufferCompleted && starter_frame_counter == MAX_FRAMES_IN_FLIGHT) {
            firstFramebufferCompleted = true;
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


void Engine::updateBufferMapped_uniformDeltaTime(uint32_t currentImage) {
    StructDeltaTime ubo{};
    ubo.deltaTime = static_cast<glm::float32>(lastFrameTime);
    memcpy(bfr.bufferMapped_uniformDeltaTime[currentImage], &ubo, sizeof(ubo));
}

void Engine::updateBufferMapped_uniformMVP(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    StructMVP ubo{};
    ubo.model = glm::translate(
        glm::rotate(
            glm::mat4(1.0f), 
            0.03f * time * glm::radians(90.0f), 
            glm::vec3(0.0f, 0.0f, 1.0f)
        ),
        -glm::vec3(0.5f,0.5f,0.5f)
    );
    ubo.view = glm::lookAt(glm::vec3(1.8f, 1.8f, 1.8f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swp.swapChainExtent.width / (float)swp.swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(bfr.bufferMapped_uniformMVP[currentImage], &ubo, sizeof(ubo));
}

void Engine::updateBufferMapped_storageTransformtions(uint32_t currentImage) {
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
    
    if (ENABLE_PHYSICS) {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline_particles);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout_particles, 0, 1, &dscr.descriptorSets_uniformMVP[currentFrame], 0, nullptr);

        // draw compute on gfx particle pipeline

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &bfr.buffer_physics_particles[currentFrame], offsets);

        vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

        vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);
    }

    // draw lines;
    if(ENABLE_LVBH) {
        //  draw points
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline_particles);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout_particles, 0, 1, &dscr.descriptorSets_uniformMVP[currentFrame], 0, nullptr);

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &bfr.buffer_lbvh_particles, offsets);

        vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

        vkCmdDraw(commandBuffer, NUM_ELEMENTS, 1, 0, 0);
        // --------------------

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline_lines);

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &bfr.buffer_lines, offsets);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout_lines, 0, 1, &dscr.descriptorSets_uniformMVP[currentFrame], 0, nullptr);
        vkCmdSetLineWidth(commandBuffer, 1.0f);
        vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        vkCmdDraw(commandBuffer, NUM_BB_POINTS, 1, 0, 0);
    }

    if (ENABLE_POLY)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.graphicsPipeline);

        VkBuffer vertexBuffers3[] = { bfr.vertexBuffer };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers3, offsets);

        vkCmdBindIndexBuffer(commandBuffer, bfr.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipelineLayout, 0, 1, &dscr.descriptorSets_multi_MPV_TS_TRN[currentFrame], 0, nullptr);

        vkCmdSetPrimitiveTopologyEXT(commandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(bfr.indices.size()), 1, 0, 0, 0);
    }

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

    if (ENABLE_PHYSICS) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePipeline);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePipelineLayout, 0, 1, &dscr.descriptorSets_1UC_4SC[currentFrame], 0, nullptr);

        vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);

        VkMemoryBarrier memoryBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, VK_NULL_HANDLE, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrier, 0, nullptr, 0, nullptr);

    }
    
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
}

void Engine::recordLBVHComputeCommandBuffer(VkCommandBuffer commandBuffer, bool recalculate)
{

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording LBVH compute command buffer!");
    }
    // ====================== LBVH ========================
    const StructDeltaTimeLBVH pushDeltaTime{ lastFrameTime, NUM_ELEMENTS , 1.0f };//lastTime//lastFrameTime
    uint32_t size = static_cast<uint32_t>(dscr.descriptorSets_lbvh.size());
    uint32_t x = (NUM_ELEMENTS + 256 - 1) / 256;
    uint32_t y = (256 + 256 - 1) / 256;

    if (!false) {
        // ------ update particle positions from compute shader. for demonstration.
        
        vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_particles_update, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(StructDeltaTimeLBVH), &pushDeltaTime);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_particles_update);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_particles_update, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);
        vkCmdDispatch(commandBuffer, x, 1, 1);

        VkMemoryBarrier memoryBarrierZ{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrierZ, 0, nullptr, 0, nullptr);
    }
    if (recalculate) {
        if (!false) {
            // ---------------- Update Bounding Boxes ---------------
            vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_bounding_box_update, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(StructDeltaTimeLBVH), &pushDeltaTime);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_bounding_box_update);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_bounding_box_update, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);
            vkCmdDispatch(commandBuffer, x, 1, 1);

            VkMemoryBarrier memoryBarrierX{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrierX, 0, nullptr, 0, nullptr);
        }
        // -------------- Generate Morton Codes ---------------

        vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_morton_codes, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstantsMortonCodes), &pushConstMC);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_morton_codes);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_morton_codes, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);

        vkCmdDispatch(commandBuffer, x, 1, 1);

        VkMemoryBarrier memoryBarrier0{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrier0, 0, nullptr, 0, nullptr);

        // --------------------- Radix Sort ------------------------

        vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_single_radixsort, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstantsRadixSort), &pushConstRS);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_single_radixsort);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_single_radixsort, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);

        vkCmdDispatch(commandBuffer, y, 1, 1);

        VkMemoryBarrier memoryBarrier1{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrier1, 0, nullptr, 0, nullptr);

        // ------------------ Calculate Hierarchy -------------------


        vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_hierarchy, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstantsHierarchy), &pushConstHierarchy);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_hierarchy);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_hierarchy, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);

        vkCmdDispatch(commandBuffer, x, 1, 1);

        VkMemoryBarrier memoryBarrier2{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrier2, 0, nullptr, 0, nullptr);


        // -------------- Calculate Bounding Boxes -----------------

        vkCmdPushConstants(commandBuffer, cmpt.computePL_lbvh_bounding_boxes, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstantsBoundingBoxes), &pushConstantsBoundingBoxes);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computeP_lbvh_bounding_boxes);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmpt.computePL_lbvh_bounding_boxes, 0, size, dscr.descriptorSets_lbvh.data(), 0, nullptr);

        vkCmdDispatch(commandBuffer, x, 1, 1);

        VkMemoryBarrier memoryBarrier3{ VK_STRUCTURE_TYPE_MEMORY_BARRIER, NULL, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, {}, 1, &memoryBarrier3, 0, nullptr, 0, nullptr);

        // --------------- copy LBVH to host-visible buffer.
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = sizeof(LBVHNode) * NUM_LBVH_ELEMENTS;

        vkCmdCopyBuffer(commandBuffer, bfr.buffer_lbvh_LBVH, bfr.buffer_lbvh_LBVH_host_vis, 1, &copyRegion);
    }

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record LBVH compute command buffer!");
    }
   
}

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
    std::cout << "frm:" << cnt << std::endl;
    VkSubmitInfo submitInfo{};
    if (ENABLE_PHYSICS) {
        
        vkWaitForFences(device, 1, &sync.computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        updateBufferMapped_uniformDeltaTime(currentFrame);
        vkResetFences(device, 1, &sync.computeInFlightFences[currentFrame]);
        
        vkResetCommandBuffer(cmd.computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordComputeCommandBuffer(cmd.computeCommandBuffers[currentFrame]);

        submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd.computeCommandBuffers[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &sync.computeFinishedSemaphores[currentFrame];
   
        VkResult resultCompute = vkQueueSubmit(computeQueue, 1, &submitInfo, sync.computeInFlightFences[currentFrame]);
        if (resultCompute != VK_SUCCESS) {
            std::cout << "Error: " << resultCompute << std::endl;
            throw std::runtime_error("failed to submit compute command buffer!");
        };
    }

    recalculateLBVH = cnt % 100 == 0;
    if (ENABLE_LVBH) {
        vkWaitForFences(device, 1, &sync.lbvhComputeFence, VK_TRUE, UINT64_MAX);

        vkResetFences(device, 1, &sync.lbvhComputeFence);
        vkResetCommandBuffer(cmd.commandLBVHComputeBuffer, /*VkCommandBufferResetFlagBits*/ 0);
        recordLBVHComputeCommandBuffer(cmd.commandLBVHComputeBuffer, recalculateLBVH);

        submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd.commandLBVHComputeBuffer;
        //submitInfo.signalSemaphoreCount = 1;
        //submitInfo.pSignalSemaphores = &sync.lbvhComputeSemaphore;

        VkResult resultLBVHCompute = vkQueueSubmit(computeQueue, 1, &submitInfo, sync.lbvhComputeFence);
        if (resultLBVHCompute != VK_SUCCESS) {
            std::cout << "Error: " << resultLBVHCompute << std::endl;
            throw std::runtime_error("failed to submit LBVH compute command buffer!");
        };
        vkQueueWaitIdle(computeQueue);

        // ---------------- Draw bounding boxes!!! ------------------------
        // update line buffer only if they were recalcualted
        if (recalculateLBVH) {
            std::vector<LBVHNode> nodes(NUM_LBVH_ELEMENTS); // Allocate vector to hold the copied data
            VkDeviceSize bSize = sizeof(LBVHNode) * NUM_LBVH_ELEMENTS;
            std::memcpy(nodes.data(), bfr.bufferMapped_lbvh_LBVH_hist_vis, sizeof(LBVHNode) * NUM_LBVH_ELEMENTS);

            std::vector<VertexBase> verts;
            verts.reserve(NUM_BB_POINTS);

            BBox::getVerts(
                glm::vec3(1.0f, 0.0f, 0.0f),
                pushConstMC.g_min_x,
                pushConstMC.g_min_y,
                pushConstMC.g_min_z,
                pushConstMC.g_max_x,
                pushConstMC.g_max_y,
                pushConstMC.g_max_z,
                verts
            );

            for (size_t i = 0; i < NUM_ELEMENTS - 1; i++)
            {
                BBox::getVerts(
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    nodes[i].aabbMinX,
                    nodes[i].aabbMinY,
                    nodes[i].aabbMinZ,
                    nodes[i].aabbMaxX,
                    nodes[i].aabbMaxY,
                    nodes[i].aabbMaxZ,
                    verts
                );
            }

            std::memcpy(bfr.bufferMapped_lines, verts.data(), sizeof(VertexBase) * NUM_BB_POINTS);
        }
    }
    
    // Graphics submission

    vkWaitForFences(device, 1, &sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    if (ENABLE_POLY) {
        updateBufferMapped_storageTransformtions(currentFrame);
    }
    updateBufferMapped_uniformMVP(currentFrame);

    uint32_t imageIndex;
    VkResult acquireNextImageResult = vkAcquireNextImageKHR(device, swp.swapChain, UINT64_MAX, sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &sync.inFlightFences[currentFrame]);

    vkResetCommandBuffer(cmd.commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(cmd.commandBuffers[currentFrame], imageIndex);

    std::vector<VkSemaphore> waitSemaphores  ={ sync.imageAvailableSemaphores[currentFrame] };
    std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
     
    /*if (ENABLE_LVBH && executeLBVH) {
        waitSemaphores.push_back(sync.lbvhComputeSemaphore);
        waitStages.push_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    }*/
    if (ENABLE_PHYSICS) {
        waitSemaphores.push_back(sync.computeFinishedSemaphores[currentFrame]);
        waitStages.push_back(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
    }
   
    submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd.commandBuffers[currentFrame];

    std::vector<VkSemaphore> signalSemaphores = { sync.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    VkResult queueSubmitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, sync.inFlightFences[currentFrame]);
    if (queueSubmitResult != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores.data();

    std::vector < VkSwapchainKHR> swapChains = { swp.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains.data();

    presentInfo.pImageIndices = &imageIndex;

    VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || glfw_s.framebufferResized) {
        glfw_s.framebufferResized = false;
        recreateSwapChain();
    }
    else if (queuePresentResult != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    cnt++;
    //if (cnt >= 300) {
    //    vkDeviceWaitIdle(device);
    //}
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

