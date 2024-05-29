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

std::string exec(std::string cmd) {
   std::array<char, 128> buffer;
   std::string result;
   std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                 pclose);
   if (!pipe) {
      throw std::runtime_error("popen() failed!");
   }
   while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
          nullptr) {
      result += buffer.data();
   }
   return result;
}

std::string prepare_file_path() {
   std::string final_path = "realpath " + smot::args.path_to_screenshots;
   final_path = exec(final_path.c_str());

   final_path.replace(final_path.length() - 1, 1, "/");
   std::string result = exec("date");

   result.replace(result.length() - 1, 1, "");
   std::replace(result.begin(), result.end(), ' ', '-');

   final_path += "screenshot-" + result;

   while (exec("file " + final_path + ".png")
              .find("(No such file or directory)") == std::string::npos) {
      final_path += "_1";
   }

   final_path += +".png";

   return final_path;
}

void initalize_imlib_for_screenshot(XEnvironment &x_env) {
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
}

Imlib_Image take_screenshot(XEnvironment &x_env, XRectangle capture_area) {
   Imlib_Image screenshot = imlib_create_image_from_drawable(
       0, capture_area.x, capture_area.y, capture_area.width,
       capture_area.height, 1);
   return screenshot;
}

int32_t save_screenshot(XEnvironment &x_env, XRectangle capture_area,
                        Imlib_Image screenshot) {
   imlib_context_set_image(screenshot);
   std::string filename = prepare_file_path();

   if (smot::args.verbose_mode) {
      std::cout << filename << std::endl;
   }

   Imlib_Load_Error imErr;
   imlib_save_image_with_error_return(filename.c_str(), &imErr);
   if (imErr) {
      std::cout << "Oh no! Unable to save Screenshot" << std::endl;
   }

   imlib_free_image_and_decache();
   return 0;
}
