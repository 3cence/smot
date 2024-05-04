#ifndef SCREENCAPTURE_HEADER
#define SCREENCAPTURE_HEADER

#include "Graphics.h"
#include "Smot.h"
#include <X11/Xlib.h>

void take_and_save_screenshot(XEnvironment &, ProgramArgs, XRectangle);

#endif
