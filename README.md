# Vulkan_introduction_tutorial

take-aways:
* (23/10/24) Learned about static methods, which allow you use class methods without "spawning" class instances. THIS IS A GAMECHANGER!
* (24/10/24) GPU has many levels of memory access for CPU. Shaders need data on GPU. "Closest" memory to CPU is "host-visible". In tutorial mesh was loaded into device-local memory. We displayed transformed vertex data by applying matrix transformation onto initial geometry. Transformation matrices were stored in Uniform buffers on hist-visible memory.
* (25/10/24) Descriptor Set Layout hold info about collection of memory resources available for shaders. Layout contains following info for each resources: index (binding), resource type and which shaders stages have acces to it (stageFlags)

Challenges and approaches to solution, sort of evolution:

go through https://vulkan-tutorial.com/Introduction and start chaping your own "engine".

*   Challenge 1: Tutorial ends up in one file. One class. A lot of class parameters. Want to split it into many bits. How do you share parameters that are comonly used by different parts?
*   Solution to 1: Define many smaller classes and point their common parameters to same master parameter.
*   Challenge 2: Issue with 1: have to manually syncronize many parameters. Introduce one more parameter-> add everywhere for sync.
*   Solution to 2: synchronize, not individual parameters, but, whole classes.
    class param-> std::shared_ptr<BaseEnvironment> base; -> spawn in world with unique pointer: std::shared_ptr<BaseEnvironment> pBase = std::make_shared<BaseEnvironment>(); -> link inside class: base = pBase.
*   Challenge 3: Issue with 2: Each class spawns its own version of other class. Very likely it makes things very slow. All for convenience of not having to share parameters via function arguments.
*   Why this trajectory? Hard to transition from having only one class. Its convenient to have explicit access to variables. Engine is very big ~1500 lines, maybe? Hard to understand which parameters are commonly used by which modules.
*   Solution to 3: Now its more clear which parameters are central, it is easier to restructure modules hierarchically. 

https://www.reddit.com/r/vulkan/comments/19agm4z/vulkan_api_walkthrough_examples_you_can_learn/
https://johannesugb.github.io/gpu-programming/setting-up-a-proper-vulkan-projection-matrix/

later read. i think at some points you should "hide" includes from main.cpp inside other ccp's.
It worked for stbi, and probly for tiny_obj_loader.
https://stackoverflow.com/questions/43348798/double-inclusion-and-headers-only-library-stbi-image


comments for simple particles:
goal- draw particles as individual vertices.

need particle parameters in a struct.

have to store particle data

1) where they will be stored ? 
Storage case 1 (my easy):
Storage buffer - can be modified.
Storage case 2:
Device local memory, can be modified by compute shaders.

Lets make a storage buffer for vertex shader access (MPV transformation only) and fragment shader for coloring.
Need descriptor set layout first VK_DESCRIPTOR_TYPE_STORAGE_BUFFER-VK_SHADER_STAGE_VERTEX_BIT.
createDescriptorSets_StorageParticles - .range = sizeof(point3D) * PARTICLE_COUNT;
create buffer - 

