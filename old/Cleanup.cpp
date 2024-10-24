#include "Cleanup.h"

void Cleanup::connect(std::shared_ptr<Device> env, std::shared_ptr<Image> env2, std::shared_ptr<Resources> env3) { dvc = env, img = env2, res = env3; };

void Cleanup::cleanupSwapChain() {
    img->cleanupSwapChain();
    res->cleanupSwapChain();
}

void Cleanup::cleanup() {
    timer.line_init("cleanup");
    cleanupSwapChain();
    img->cleanup();
    res->cleanup();
    dvc->cleanup();
    //deb->cleanup(base); // get error- base became NULL
    base->cleanup();
    timer.line_end("cleanup");
}