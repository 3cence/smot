#include "headers/Smot.h"
#include "headers/graphics.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

int32_t load_args(int argc, char **argv, ProgramArgs *args) {
  bool scrot_folder_provided = false;
  args->verbose_mode = false;
  for (int i = 1; i < argc; i++) {
    std::string arg = std::string(argv[i]);
    if (arg == "-v") {
      args->verbose_mode = true;
    } else if (arg[0] != '-') {
      scrot_folder_provided = true;
      args->path_to_screenshots = std::string(argv[i]);
    } else {
      std::cerr << "smot [options] folder_path" << std::endl;
      std::cerr << "[options]" << std::endl;
      std::cerr << "-v : verbose mode, for debugging" << std::endl;
      return 1;
    }
  }
  if (!scrot_folder_provided) {
    args->path_to_screenshots = "~/Images/screenshots/";
  }
  return 0;
}

std::string build_scrot_command(ProgramArgs args, XRectangle scrot_rectangle) {
  std::string file_name_template = "%Y-%m-%d-%T-screenshot.png";
  std::string options = "-a " + std::to_string(scrot_rectangle.x) + "," +
                        std::to_string(scrot_rectangle.y) + "," +
                        std::to_string(scrot_rectangle.width) + "," +
                        std::to_string(scrot_rectangle.height) + " ";

  std::string command =
      "scrot " + options + " " + args.path_to_screenshots + file_name_template;

  if (args.verbose_mode) {
    std::cout << "Scrot Rectangle: " << std::endl;
    std::cout << "[ " << scrot_rectangle.x << ", " << scrot_rectangle.y << " ]"
              << std::endl;
    std::cout << "[ " << scrot_rectangle.width << ", " << scrot_rectangle.height
              << " ]" << std::endl;
    std::cout << command << std::endl;
  }
  return command;
}

int main(int argc, char **argv) {
  ProgramArgs args;
  int32_t valid_args = load_args(argc, argv, &args);
  if (valid_args != 0)
    return 1;

  XEnvironment x_env;
  int32_t x_env_status = open_x_environment(x_env, args);
  if (x_env_status != 0) {
    return 1;
  }

  bool select_mode_active = true;
  XRectangle scrot_rectangle;
  memset(&scrot_rectangle, 0, sizeof(XRectangle));

  int32_t number_of_clicks = 0;
  while (select_mode_active) {
    XSelectInput(x_env.display, x_env.root_window, ButtonReleaseMask);
    XEvent event;
    XNextEvent(x_env.display, &event);

    if (event.type == ButtonRelease) {
      switch (event.xbutton.button) {
      case LeftMouseButton:
        if (number_of_clicks == 0) {
          scrot_rectangle.x = event.xbutton.x_root;
          scrot_rectangle.y = event.xbutton.y_root;
          number_of_clicks++;
        } else if (number_of_clicks == 1) {
          scrot_rectangle.width = static_cast<unsigned short>(
              event.xbutton.x_root - scrot_rectangle.x);
          scrot_rectangle.height = static_cast<unsigned short>(
              event.xbutton.y_root - scrot_rectangle.y);
          number_of_clicks++;
          select_mode_active = false;
        }
        break;
      case RightMouseButton:
        select_mode_active = false;
        break;
      default:
        break;
      }
    }

    XClearArea(x_env.display, x_env.root_window, 100, 100, 1000, 1000, false);
  }

  std::string command = build_scrot_command(args, scrot_rectangle);
  system(command.c_str());

  XUngrabPointer(x_env.display, CurrentTime);
  XCloseDisplay(x_env.display);
  return 0;
}
