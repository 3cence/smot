#include "Screencapture.h"
#include "Smot.h"
#include <Imlib2.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

std::string exec(const char *cmd) {
   std::array<char, 128> buffer;
   std::string result;
   std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
   if (!pipe) {
      throw std::runtime_error("popen() failed!");
   }
   while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
          nullptr) {
      result += buffer.data();
   }
   return result;
}

std::string prepare_file_path(ProgramArgs args) {
   std::string final_path = "realpath " + args.path_to_screenshots;
   final_path = exec(final_path.c_str());

   final_path.replace(final_path.length() - 1, 1, "/");
   std::string result = exec("date");

   result.replace(result.length() - 1, 1, "");
   std::replace(result.begin(), result.end(), ' ', '-');

   final_path += "screenshot-" + result + ".png";

   return final_path;
}

void take_and_save_screenshot(XEnvironment &x_env, ProgramArgs args,
                              XRectangle capture_area) {
   // Initalize imlib. This should be moved to open_x_environment()
   int scr = XDefaultScreen(x_env.display);
   Visual *vis = DefaultVisual(x_env.display, scr);
   Colormap cm = DefaultColormap(x_env.display, scr);

   imlib_context_set_drawable(x_env.root_window);
   imlib_context_set_display(x_env.display);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);
   imlib_set_cache_size(0);

   Imlib_Image screenshot = imlib_create_image_from_drawable(
       0, capture_area.x, capture_area.y, capture_area.width,
       capture_area.height, 1);
   imlib_context_set_image(screenshot);

   std::string filename = prepare_file_path(args);
   // std::string filename = "screenshot.png";
   std::cout << filename << std::endl;
   Imlib_Load_Error imErr;
   imlib_save_image_with_error_return(filename.c_str(), &imErr);
   if (imErr) {
      std::cout << "Oh no! Unable to save Screenshot" << std::endl;
   }
}
