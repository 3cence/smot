#ifndef SMOT_HEADER
#define SMOT_HEADER
#include <X11/Xlib.h>
#include <stdint.h>
#include <string>

enum MouseEvent {
  NoEvent = 0,
  LeftMouseButton = 1,
  MiddleMouseButton = 2,
  RightMouseButton = 3,
  BackMouseButton = 8,
  FrontMouseButton = 9
};

struct ProgramArgs {
  bool verbose_mode, disable_visual;
  std::string path_to_screenshots;
};

#endif
