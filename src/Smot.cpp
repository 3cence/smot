#include "Smot.h"
#include "Graphics.h"
#include "Screencapture.h"
#include <Imlib2.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

ProgramArgs smot::args;
int32_t load_args(int argc, char **argv);
XRectangle get_scrot_region(XPoint anchor, XPoint secondary);

int main(int argc, char **argv) {
   int32_t valid_args = load_args(argc, argv);
   if (valid_args != 0)
      return 1;

   XEnvironment x_env;
   int32_t x_env_status = open_x_environment(x_env);
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
   XPoint anchor_point = {0, 0};
   XPoint secondary_point = {-1, -1};
   int32_t number_of_clicks = 0;

   while (select_mode_active) {
      XSelectInput(x_env.display, x_env.root_window,
                   ButtonReleaseMask | PointerMotionMask);
      XEvent event;
      XNextEvent(x_env.display, &event);

      if (event.type == ButtonRelease) {
         switch (event.xbutton.button) {
         case ME_LeftMouseButton:
            if (number_of_clicks == 0) {
               anchor_point.x = event.xbutton.x_root;
               anchor_point.y = event.xbutton.y_root;
               number_of_clicks++;
               if (!smot::args.disable_visual) {
                  begin_area_selection(
                      x_env, get_scrot_region(anchor_point, anchor_point));
               }
            } else if (number_of_clicks == 1) {
               secondary_point.x = event.xbutton.x_root;
               secondary_point.y = event.xbutton.y_root;
               number_of_clicks++;
               if (!smot::args.disable_visual) {
                  update_area_selection(
                      x_env, get_scrot_region(anchor_point, secondary_point));
               }
               select_mode_active = false;
            }
            break;
         case ME_RightMouseButton:
            select_mode_active = false;
            canceled = true;
            break;
         default:
            break;
         }
      } else if (event.type == MotionNotify) {
         if (number_of_clicks == 1 && !smot::args.disable_visual) {
            secondary_point.x = event.xbutton.x_root;
            secondary_point.y = event.xbutton.y_root;
            update_area_selection(
                x_env, get_scrot_region(anchor_point, secondary_point));
         }
      }
   }

   XUngrabPointer(x_env.display, CurrentTime);

   if (!canceled) {
      initalize_imlib_for_screenshot(x_env);
      XRectangle region = get_scrot_region(anchor_point, secondary_point);
      Imlib_Image screenshot = take_screenshot(region);
      save_screenshot(screenshot);
   }

   close_x_environment(x_env);

   return 0;
}

int32_t load_args(int argc, char **argv) {
   bool scrot_folder_provided = false;
   smot::args.verbose_mode = false;
   smot::args.disable_visual = false;
   smot::args.video_mode = false;
   for (int i = 1; i < argc; i++) {
      std::string arg = std::string(argv[i]);
      if (arg == "-v") {
         smot::args.verbose_mode = true;
      } else if (arg == "-n") {
         smot::args.disable_visual = true;
      } else if (arg == "-r") {
         smot::args.video_mode = true;
      } else if (arg[0] != '-') {
         scrot_folder_provided = true;
         // this does not work. ill probably fix it at some point
         // if (arg.end()[arg.length() - 1] != '/') {
         //   smot::args.path_to_screenshots = std::string(argv[i]) + "/";
         // }
         smot::args.path_to_screenshots = std::string(argv[i]);
      } else {
         std::cerr << "smot [options] folder_path" << std::endl;
         std::cerr << "[options]" << std::endl;
         std::cerr << "-v : verbose mode, for debugging" << std::endl;
         return 1;
      }
   }
   if (!scrot_folder_provided) {
      smot::args.path_to_screenshots = "~/Images/screenshots/";
   }
   return 0;
}

XRectangle get_scrot_region(XPoint anchor_point, XPoint secondary_point) {
   if (anchor_point.x == secondary_point.x) {
      secondary_point.x++;
   }
   if (anchor_point.y == secondary_point.y) {
      secondary_point.y++;
   }
   XRectangle region;
   // Quad 1
   if (anchor_point.x < secondary_point.x &&
       anchor_point.y > secondary_point.y) {
      region.x = anchor_point.x;
      region.y = secondary_point.y;
      region.width = secondary_point.x - anchor_point.x;
      region.height = anchor_point.y - secondary_point.y;
   }
   // Quad 2
   else if (anchor_point.x > secondary_point.x &&
            anchor_point.y > secondary_point.y) {
      region.x = secondary_point.x;
      region.y = secondary_point.y;
      region.width = anchor_point.x - secondary_point.x;
      region.height = anchor_point.y - secondary_point.y;
   }
   // Quad 3
   else if (anchor_point.x > secondary_point.x &&
            anchor_point.y < secondary_point.y) {
      region.x = secondary_point.x;
      region.y = anchor_point.y;
      region.width = anchor_point.x - secondary_point.x;
      region.height = secondary_point.y - anchor_point.y;
   }
   // Quad 4
   else if (anchor_point.x < secondary_point.x &&
            anchor_point.y < secondary_point.y) {
      region.x = anchor_point.x;
      region.y = anchor_point.y;
      region.width = secondary_point.x - anchor_point.x;
      region.height = secondary_point.y - anchor_point.y;
   }
   return region;
}
