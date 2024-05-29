#include "Graphics.h"
#include "Smot.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <iostream>

#define SCROT_BORDER_WIDTH 5

int32_t open_x_environment(XEnvironment &x_env) {
   x_env.display = XOpenDisplay(NULL);
   if (x_env.display == NULL) {
      std::cerr << "Failed to open display" << std::endl;
      return 1;
   } else if (smot::args.verbose_mode) {
      std::cout << "Display opened successfully" << std::endl;
   }

   x_env.root_window = XDefaultRootWindow(x_env.display);

   // Set window attributes
   // Will need to experiement: but may need to disable mouse event
   // propigation
   XVisualInfo visual_info;
   XMatchVisualInfo(x_env.display, DefaultScreen(x_env.display), 32, TrueColor,
                    &visual_info);

   unsigned long attribute_mask = 0;
   XSetWindowAttributes attributes;

   attribute_mask |= CWColormap;
   attributes.colormap =
       XCreateColormap(x_env.display, DefaultRootWindow(x_env.display),
                       visual_info.visual, AllocNone);
   attribute_mask |= CWOverrideRedirect;
   attributes.override_redirect = True;
   attribute_mask |= CWBorderPixel;
   attributes.border_pixel = 0x9acd83FF;
   attribute_mask |= CWBackPixel;
   attributes.background_pixel = 0x0;
   attribute_mask |= CWSaveUnder;
   attributes.save_under = True;

   x_env.display_window =
       XCreateWindow(x_env.display, x_env.root_window, 0, 0, 100, 100,
                     SCROT_BORDER_WIDTH, visual_info.depth, InputOutput,
                     visual_info.visual, attribute_mask, &attributes);

   // Map window to the screen -- for testing
   // XMapRaised(x_env.display, x_env.display_window);

   return 0;
}

void begin_area_selection(XEnvironment &x_env, XRectangle selection_area) {
   update_area_selection(x_env, selection_area);
   XMapRaised(x_env.display, x_env.display_window);
}

void update_area_selection(XEnvironment &x_env, XRectangle selection_area) {
   unsigned int mask = 0;
   XWindowChanges changes;

   mask |= CWX | CWY | CWWidth | CWHeight;
   changes.x = selection_area.x - SCROT_BORDER_WIDTH;
   changes.y = selection_area.y - SCROT_BORDER_WIDTH;
   changes.width = selection_area.width;
   changes.height = selection_area.height;

   // XLowerWindow(x_env.display, x_env.display_window);
   XConfigureWindow(x_env.display, x_env.display_window, mask, &changes);
   // XRaiseWindow(x_env.display, x_env.display_window);
   // XEvent e;
   // XSendEvent(x_env.display, x_env.root_window, False, ExposureMask, &e);
}

void close_x_environment(XEnvironment &x_env) {
   XDestroyWindow(x_env.display, x_env.display_window);
   XCloseDisplay(x_env.display);
}
