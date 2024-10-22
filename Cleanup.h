#pragma once

#ifndef CLEANUP_H
#define CLEANUP_H

#include "Timer.h"
#include <memory>
#include "BaseEnvironment.h"
#include "DebugEnvironment.h"
#include "Resources.h"
#include "Device.h"
#include "Image.h"


class Cleanup { // order of cleanup is not perfect. if ur srs take great care.
private:
    std::shared_ptr<BaseEnvironment> base;
    std::shared_ptr<DebugEnvironment> deb;
    std::shared_ptr<Device> dvc;
    std::shared_ptr<Image> img;
    std::shared_ptr<Resources> res;
    Timer timer;
public:
    Cleanup(std::shared_ptr<BaseEnvironment> env) : base(env) {}

    void connect(std::shared_ptr<Device> env, std::shared_ptr<Image> env2, std::shared_ptr<Resources> env3);
    void cleanupSwapChain();
    void cleanup();
};

#endif
