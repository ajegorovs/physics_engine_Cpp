#pragma once
#include <cstdint>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Window dimensions
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Number of frames to be processed concurrently
const int MAX_FRAMES_IN_FLIGHT = 2;


