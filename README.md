# Vulkan_introduction_tutorial

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


