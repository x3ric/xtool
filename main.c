#include "xtool.h"

Display *display;
Window root;
int xi_opcode;

void print_usage() {
    printf("Usage:\n"
        "  xtool [command] [args...]\n\n"
        "Commands:\n"
        "  inputscan                     Listen input\n"
        "  fakemouse                     Control the mouse with numpad\n"
        "  mousemove x y                 Move the mouse to absolute coordinates\n"
        "  mousemove_smoth x y           Move the mouse to absolute coordinates smoth\n"
        "  mousemove_relative x y        Move the mouse relative to its current position\n"
        "  mousemove_relative_smoth x y  Move the mouse relative to its current position smoth\n"
        "  click button_number           Simulate a mouse click (1=left, 2=middle, 3=right)\n"
        "  mousedown button_number       Simulate pressing a mouse button\n"
        "  mouseup button_number         Simulate releasing a mouse button\n"
        "  key key_name                  Simulate a key press and release\n"
        "  keydown key_name              Simulate a key press\n"
        "  keyup key_name                Simulate a key release\n"
        "  type string                   Type a string\n"
        "  getmouselocation              Print the current mouse location\n"
        "  windowfocus window_id         Focus on a specific window\n"
        "  windowactivate window_id      Activate a specific window\n"
        "  windowsize window_id w h      Set the size of a window\n"
        "  windowmove window_id x y      Move a window to a specific location\n"
        "  getactivewindow               Print the ID of the currently active window\n"
        "  search string                 Search for a window with a title containing the string\n"
        "  listwindows                   List all visible windows\n\n"
        "Examples:\n"
        "  xtool mousemove 100 100\n"
        "  xtool click 1\n"
        "  xtool key Return\n"
        "  xtool type \"Hello, World!\"\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }
    init_x();
    if (strcmp(argv[1], "mousemove") == 0) {
        if (argc != 4) {
            fprintf(stderr, "mousemove requires x and y arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        simulate_mouse_movement(x, y, true, false);
    } else if (strcmp(argv[1], "mousemove_relative") == 0) {
        if (argc != 4) {
            fprintf(stderr, "mousemove_relative requires x and y arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        simulate_mouse_movement(x, y, false, false);
    } else if (strcmp(argv[1], "mousemove_smoth") == 0) {
        if (argc != 4) {
            fprintf(stderr, "mousemove requires x and y arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        simulate_mouse_movement(x, y, true, true);
    } else if (strcmp(argv[1], "mousemove_relative_smoth") == 0) {
        if (argc != 4) {
            fprintf(stderr, "mousemove_relative requires x and y arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        int x = atoi(argv[2]);
        int y = atoi(argv[3]);
        simulate_mouse_movement(x, y, false, true);
    } else if (strcmp(argv[1], "click") == 0) {
        if (argc != 3) {
            fprintf(stderr, "click requires a button number argument\n");
            close_x();
            return EXIT_FAILURE;
        }
        int button = atoi(argv[2]);
        simulate_mouse_click(button, True);
        delay(10000);
        simulate_mouse_click(button, False);
    } else if (strcmp(argv[1], "mousedown") == 0) {
        if (argc != 3) {
            fprintf(stderr, "mousedown requires a button number argument\n");
            close_x();
            return EXIT_FAILURE;
        }
        int button = atoi(argv[2]);
        simulate_mouse_click(button, True);
    } else if (strcmp(argv[1], "mouseup") == 0) {
        if (argc != 3) {
            fprintf(stderr, "mouseup requires a button number argument\n");
            close_x();
            return EXIT_FAILURE;
        }
        int button = atoi(argv[2]);
        simulate_mouse_click(button, False);
    } else if (strcmp(argv[1], "key") == 0 || 
               strcmp(argv[1], "keydown") == 0 || 
               strcmp(argv[1], "keyup") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s requires a key name argument\n", argv[1]);
            close_x();
            return EXIT_FAILURE;
        }
        KeySym keysym = XStringToKeysym(argv[2]);
        if (keysym == NoSymbol) {
            fprintf(stderr, "Invalid key name: %s\n", argv[2]);
            close_x();
            return EXIT_FAILURE;
        }
        KeyCode keycode = XKeysymToKeycode(display, keysym);
        if (strcmp(argv[1], "key") == 0) {
            simulate_key_press(keycode, True);
            delay(10000);
            simulate_key_press(keycode, False);
        } else if (strcmp(argv[1], "keydown") == 0) {
            simulate_key_press(keycode, True);
        } else if (strcmp(argv[1], "keyup") == 0) {
            simulate_key_press(keycode, False);
        }
    } else if (strcmp(argv[1], "type") == 0) {
        if (argc != 3) {
            fprintf(stderr, "type requires a string argument\n");
            close_x();
            return EXIT_FAILURE;
        }
        type_string(argv[2]);
    } else if (strcmp(argv[1], "inputscan") == 0) {
        inputscan();
    } else if (strcmp(argv[1], "fakemouse") == 0) {
        fakemouse();
    } else if (strcmp(argv[1], "getmouselocation") == 0) {
        int x, y;
        get_mouse_position(&x, &y);
        printf("x:%d y:%d\n", x, y);
    } else if (strcmp(argv[1], "windowfocus") == 0 || strcmp(argv[1], "windowactivate") == 0) {
        if (argc != 3) {
            fprintf(stderr, "%s requires a window ID argument\n", argv[1]);
            close_x();
            return EXIT_FAILURE;
        }
        Window window = strtoul(argv[2], NULL, 0);
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
        XRaiseWindow(display, window);
    } else if (strcmp(argv[1], "windowsize") == 0) {
        if (argc != 5) {
            fprintf(stderr, "windowsize requires window ID, width, and height arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        Window window = strtoul(argv[2], NULL, 0);
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        XResizeWindow(display, window, width, height);
    } else if (strcmp(argv[1], "windowmove") == 0) {
        if (argc != 5) {
            fprintf(stderr, "windowmove requires window ID, x, and y arguments\n");
            close_x();
            return EXIT_FAILURE;
        }
        Window window = strtoul(argv[2], NULL, 0);
        int x = atoi(argv[3]);
        int y = atoi(argv[4]);
        XMoveWindow(display, window, x, y);
    } else if (strcmp(argv[1], "getactivewindow") == 0) {
        Window active_window = get_active_window();
        if (active_window != None) {
            printf("0x%lx\n", active_window);
        } else {
            fprintf(stderr, "No active window found\n");
        }
    } else if (strcmp(argv[1], "search") == 0) {
        if (argc != 3) {
            fprintf(stderr, "search requires a string argument\n");
            close_x();
            return EXIT_FAILURE;
        }
        Window *children;
        unsigned int nchildren;
        XQueryTree(display, root, &root, &root, &children, &nchildren);
        for (unsigned int i = 0; i < nchildren; i++) {
            char *title = get_window_title(children[i]);
            if (strstr(title, argv[2]) != NULL) {
                printf("Window ID: 0x%lx, Title: %s\n", children[i], title);
            }
            free(title);
        }
        XFree(children);
    } else if (strcmp(argv[1], "listwindows") == 0) {
        list_windows();
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_usage();
        close_x();
        return EXIT_FAILURE;
    }
    close_x();
    return EXIT_SUCCESS;
}
