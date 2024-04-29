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

struct ScrotRectangle {
  int32_t points_set;
  int32_t x1, y1;
  int32_t x2, y2;
};

struct ProgramArgs {
  bool verbose_mode;
  std::string path_to_screenshots;
};

#endif
