#include "Smot.h"
#include "Graphics.h"
#include "Screencapture.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

int32_t load_args(int argc, char **argv, ProgramArgs &args) {
   bool scrot_folder_provided = false;
   args.verbose_mode = false;
   args.disable_visual = false;
   for (int i = 1; i < argc; i++) {
      std::string arg = std::string(argv[i]);
      if (arg == "-v") {
         args.verbose_mode = true;
      } else if (arg == "-n") {
         args.disable_visual = true;
      } else if (arg[0] != '-') {
         scrot_folder_provided = true;
         // this does not work. ill probably fix it at some point
         // if (arg.end()[arg.length() - 1] != '/') {
         //   args.path_to_screenshots = std::string(argv[i]) + "/";
         // }
         args.path_to_screenshots = std::string(argv[i]);
      } else {
         std::cerr << "smot [options] folder_path" << std::endl;
         std::cerr << "[options]" << std::endl;
         std::cerr << "-v : verbose mode, for debugging" << std::endl;
         return 1;
      }
   }
   if (!scrot_folder_provided) {
      args.path_to_screenshots = "~/Images/screenshots/";
   }
   return 0;
}

XRectangle get_scrot_region(XPoint anchor, XPoint secondary) {
   if (anchor.x == secondary.x) {
      secondary.x++;
   }
   if (anchor.y == secondary.y) {
      secondary.y++;
   }
   XRectangle region;
   // Quad 1
   if (anchor.x < secondary.x && anchor.y > secondary.y) {
      region.x = anchor.x;
      region.y = secondary.y;
      region.width = secondary.x - anchor.x;
      region.height = anchor.y - secondary.y;
   }
   // Quad 2
   else if (anchor.x > secondary.x && anchor.y > secondary.y) {
      region.x = secondary.x;
      region.y = secondary.y;
      region.width = anchor.x - secondary.x;
      region.height = anchor.y - secondary.y;
   }
   // Quad 3
   else if (anchor.x > secondary.x && anchor.y < secondary.y) {
      region.x = secondary.x;
      region.y = anchor.y;
      region.width = anchor.x - secondary.x;
      region.height = secondary.y - anchor.y;
   }
   // Quad 4
   else if (anchor.x < secondary.x && anchor.y < secondary.y) {
      region.x = anchor.x;
      region.y = anchor.y;
      region.width = secondary.x - anchor.x;
      region.height = secondary.y - anchor.y;
   }
   return region;
}

int main(int argc, char **argv) {
   ProgramArgs args;
   int32_t valid_args = load_args(argc, argv, args);
   if (valid_args != 0)
      return 1;

   XEnvironment x_env;
   int32_t x_env_status = open_x_environment(x_env, args);
   if (x_env_status != 0) {
      return 1;
   }

   int32_t grab_status =
       XGrabPointer(x_env.display, x_env.root_window, False,
                    ButtonReleaseMask | PointerMotionMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);
   if (grab_status != GrabSuccess) {
      std::cerr << "Something went wrong while grabbing the pointer"
                << std::endl;
      return 1;
   }

   bool select_mode_active = true;
   bool canceled = false;
   XPoint anchor = {0, 0};
   XPoint secondary = {-1, -1};
   int32_t number_of_clicks = 0;
   while (select_mode_active) {
      XSelectInput(x_env.display, x_env.root_window,
                   ButtonReleaseMask | PointerMotionMask);
      XEvent event;
      XNextEvent(x_env.display, &event);

      if (event.type == ButtonRelease) {
         switch (event.xbutton.button) {
         case LeftMouseButton:
            if (number_of_clicks == 0) {
               anchor.x = event.xbutton.x_root;
               anchor.y = event.xbutton.y_root;
               number_of_clicks++;
               if (!args.disable_visual) {
                  begin_area_selection(x_env, get_scrot_region(anchor, anchor));
               }
            } else if (number_of_clicks == 1) {
               secondary.x = event.xbutton.x_root;
               secondary.y = event.xbutton.y_root;
               number_of_clicks++;
               if (!args.disable_visual) {
                  update_area_selection(x_env,
                                        get_scrot_region(anchor, secondary));
               }
               select_mode_active = false;
            }
            break;
         case RightMouseButton:
            select_mode_active = false;
            canceled = true;
            break;
         default:
            break;
         }
      } else if (event.type == MotionNotify) {
         if (number_of_clicks == 1 && !args.disable_visual) {
            secondary.x = event.xbutton.x_root;
            secondary.y = event.xbutton.y_root;
            update_area_selection(x_env, get_scrot_region(anchor, secondary));
         }
      }
   }

   XUngrabPointer(x_env.display, CurrentTime);

   if (!canceled) {
      take_and_save_screenshot(x_env, args,
                               get_scrot_region(anchor, secondary));
   }

   close_x_environment(x_env);

   return 0;
}
