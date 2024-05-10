#ifndef SMOT_HEADER
#define SMOT_HEADER
#include <X11/Xlib.h>
#include <stdint.h>
#include <string>

enum MouseEvent {
   ME_NoEvent = 0,
   ME_LeftMouseButton = 1,
   ME_MiddleMouseButton = 2,
   ME_RightMouseButton = 3,
   ME_BackMouseButton = 8,
   ME_FrontMouseButton = 9
};

enum CaptureMode { CM_Screenshot, CM_Video };

struct ProgramArgs {
   bool verbose_mode, disable_visual;
   std::string path_to_screenshots;
};

#endif
