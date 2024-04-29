#ifndef GRAPHICS_HEADER
#define GRAPHICS_HEADER
#include "Smot.h"
#include <cstdint>

struct XEnvironment {
  Display *display;
  Window root_window;
};

int32_t open_x_environment(XEnvironment &, ProgramArgs);

#endif
