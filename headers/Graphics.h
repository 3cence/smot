#ifndef GRAPHICS_HEADER
#define GRAPHICS_HEADER
#include <X11/Xlib.h>
#include <cstdint>

struct XEnvironment {
   Display *display;
   Window root_window;
   Window display_window;
};

int32_t open_x_environment(XEnvironment &);
void close_x_environment(XEnvironment &);

void begin_area_selection(XEnvironment &, XRectangle);
void update_area_selection(XEnvironment &, XRectangle);

#endif
