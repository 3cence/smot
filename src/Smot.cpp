#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#define UNUSED(x) (void)(x)

enum MouseEvent {
  NoEvent = 0,
  LeftMouseButton = 1,
  MiddleMouseButton = 2,
  RightMouseButton = 3,
  BackMouseButton = 8,
  FrontMouseButton = 9
};

struct ScrotRectangle {
  int32_t points_set;
  int32_t x1, y1;
  int32_t x2, y2;
};

struct ProgramArgs {
  bool verbose_mode;
  std::string path_to_screenshots;
};

void load_args(int argc, char **argv, ProgramArgs *args) {
  // Do it
}

int main(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);

  Display *display = XOpenDisplay("");
  if (display == NULL) {
    std::cerr << "Failed to open display" << std::endl;
    return 1;
  } else {
    std::cout << "Display opened successfully" << std::endl;
  }

  Window root_window = XDefaultRootWindow(display);

  int32_t grab_status =
      XGrabPointer(display, root_window, False, ButtonReleaseMask,
                   GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  if (grab_status != GrabSuccess) {
    std::cerr << "Something went wrong while grabbing the pointer" << std::endl;
    return 1;
  }

  bool select_mode_active = true;
  ScrotRectangle scrot_rectangle;
  memset(&scrot_rectangle, 0, sizeof(ScrotRectangle));

  while (select_mode_active) {
    XSelectInput(display, root_window, ButtonReleaseMask);
    XEvent event;
    XNextEvent(display, &event);

    if (event.type == ButtonRelease) {
      switch (event.xbutton.button) {
      case LeftMouseButton:
        if (scrot_rectangle.points_set == 0) {
          scrot_rectangle.x1 = event.xbutton.x_root;
          scrot_rectangle.y1 = event.xbutton.y_root;
          scrot_rectangle.points_set = 1;
        } else if (scrot_rectangle.points_set == 1) {
          scrot_rectangle.x2 = event.xbutton.x_root;
          scrot_rectangle.y2 = event.xbutton.y_root;
          scrot_rectangle.points_set = 2;
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
  }

  std::cout << "Scrot Rectangle: " << std::endl;
  std::cout << "[ " << scrot_rectangle.x1 << ", " << scrot_rectangle.y1 << " ]"
            << std::endl;
  std::cout << "[ " << scrot_rectangle.x2 - scrot_rectangle.x1 << ", "
            << scrot_rectangle.y2 - scrot_rectangle.y1 << " ]" << std::endl;

  std::string path_to_screenshots = "~/Images/screenshots/";
  std::string file_name_template = "%Y-%m-%d-%T-screenshot.png";
  std::string options =
      "-a " + std::to_string(scrot_rectangle.x1) + "," +
      std::to_string(scrot_rectangle.y1) + "," +
      std::to_string(scrot_rectangle.x2 - scrot_rectangle.x1) + "," +
      std::to_string(scrot_rectangle.y2 - scrot_rectangle.y1) + " ";

  std::string command =
      "scrot " + options + " " + path_to_screenshots + file_name_template;

  std::cout << command << std::endl;
  system(command.c_str());

  XUngrabPointer(display, CurrentTime);
  XCloseDisplay(display);
  return 0;
}
