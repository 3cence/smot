#ifndef SCREENCAPTURE_HEADER
#define SCREENCAPTURE_HEADER

#include "Graphics.h"
#include <Imlib2.h>
#include <X11/Xlib.h>
#include <cstdint>

void initalize_imlib_for_screenshot(XEnvironment &);
Imlib_Image take_screenshot(XRectangle);
int32_t save_screenshot(Imlib_Image);

#endif
