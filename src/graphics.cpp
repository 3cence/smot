#include "headers/graphics.h"
#include <iostream>

int32_t open_x_environment(XEnvironment &x_env, ProgramArgs args) {
  x_env.display = XOpenDisplay("");
  if (x_env.display == NULL) {
    std::cerr << "Failed to open display" << std::endl;
    return 1;
  } else if (args.verbose_mode) {
    std::cout << "Display opened successfully" << std::endl;
  }

  x_env.root_window = XDefaultRootWindow(x_env.display);

  int32_t grab_status =
      XGrabPointer(x_env.display, x_env.root_window, False, ButtonReleaseMask,
                   GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  if (grab_status != GrabSuccess) {
    std::cerr << "Something went wrong while grabbing the pointer" << std::endl;
    return 1;
  }
  return 0;
}
