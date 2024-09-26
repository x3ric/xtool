#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xinerama.h>

typedef struct {
    Window id;
    int x, y;
    unsigned int width, height;
    char *title;
} WindowInfo;


#define MAX_WINDOWS 100

extern Display *display;
extern Window root;
extern int xi_opcode;

void init_x() {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    root = DefaultRootWindow(display);
    int event, error;
    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        fprintf(stderr, "X Input extension not available.\n");
        exit(1);
    }
    XIEventMask evmask;
    unsigned char mask[(XI_LASTEVENT + 7)/8] = { 0 };
    evmask.deviceid = XIAllDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISetMask(mask, XI_RawKeyPress);
    XISetMask(mask, XI_RawKeyRelease);
    XISelectEvents(display, root, &evmask, 1);
    XSync(display, False);
}

void close_x() {
    XCloseDisplay(display);
}

void simulate_mouse_click(int button, bool is_press) {
    XTestFakeButtonEvent(display, button, is_press, CurrentTime);
    XFlush(display);
}

void simulate_key_press(KeyCode keycode, bool is_press) {
    XTestFakeKeyEvent(display, keycode, is_press, CurrentTime);
    XFlush(display);
}

void get_mouse_position(int *x, int *y) {
    Window child;
    int win_x, win_y;
    unsigned int mask;
    XQueryPointer(display, root, &root, &child, x, y, &win_x, &win_y, &mask);
}

Window get_active_window() {
    Atom active_window_atom = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *prop;
    Window active_window = None;
    if (XGetWindowProperty(display, root, active_window_atom, 0, 1, False, MAX_WINDOWS,
                           &type, &format, &nitems, &bytes_after, &prop) == Success) {
        if (prop != NULL) {
            active_window = *(Window*)prop;
            XFree(prop);
        }
    }
    return active_window;
}

char* get_window_title(Window window) {
    Atom net_wm_name = XInternAtom(display, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *prop;
    char *title = NULL;
    if (XGetWindowProperty(display, window, net_wm_name, 0, 1000, False, utf8_string, &type, &format, &nitems, &bytes_after, &prop) == Success) {
        if (prop != NULL) {
            title = strdup((char*)prop);
            XFree(prop);
        }
    }
    if (title == NULL) {
        char *window_name;
        if (XFetchName(display, window, &window_name) != 0) {
            title = strdup(window_name);
            XFree(window_name);
        }
    }
    return title ? title : strdup("Unknown");
}

void list_windows() {
    Window *children;
    unsigned int nchildren;
    XQueryTree(display, root, &root, &root, &children, &nchildren);
    printf("Window list:\n");
    for (unsigned int i = 0; i < nchildren; i++) {
        char *title = get_window_title(children[i]);
        XWindowAttributes attrs;
        XGetWindowAttributes(display, children[i], &attrs);
        if (attrs.map_state == IsViewable) {
            printf("Window ID: 0x%lx, Title: %s\n", children[i], title);
        }
        free(title);
    }
    XFree(children);
}

void delay(int i){
    usleep(i);
}

void type_string(const char *str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        KeySym keysym = XStringToKeysym(&str[i]);
        KeyCode keycode = XKeysymToKeycode(display, keysym);
        simulate_key_press(keycode, True);
        delay(10000);
        simulate_key_press(keycode, False);
        delay(10000);
    }
}

float sigmoid(float x) {
    return 1.0 / (1.0 + exp(-x));
}

void simulate_mouse_movement(int x, int y, bool absolute, bool smoth) {
    int cur_x, cur_y;
    Window child;
    int win_x, win_y;
    unsigned int mask;
    XQueryPointer(display, root, &root, &child, &cur_x, &cur_y, &win_x, &win_y, &mask);
    if (smoth) {
        int steps = 100;
        int sleep_duration = 5000;
        int target_x = absolute ? x : cur_x + x;
        int target_y = absolute ? y : cur_y + y;
        int start_x = cur_x;
        int start_y = cur_y;
        for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;
            float progress = sigmoid((t - 0.5) * 10);
            int new_x = start_x + (int)(progress * (target_x - start_x));
            int new_y = start_y + (int)(progress * (target_y - start_y));
            XTestFakeMotionEvent(display, -1, new_x, new_y, CurrentTime);
            XFlush(display);
            usleep(sleep_duration);
        }
    } else {
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
    }
    XFlush(display);
}

#define NANOS_PER_SEC 1000000000LL

void fakemouse() {
    XEvent event;
    float dx = 0, dy = 0;
    const float base_speed = 0.5f;
    float speed = base_speed;
    const float max_speed = 20.0f;
    const float acceleration = 0.2f;
    const float deceleration = 0.95f;
    const float lerp_factor = 0.5f;
    const long frame_time = 2000000;
    float current_x = 0, current_y = 0;
    float target_x = 0, target_y = 0;
    struct timespec start_time, end_time;
    long elapsed_time;
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        while (XPending(display) > 0) {
            XNextEvent(display, &event);
            if (event.type == GenericEvent && event.xcookie.extension == xi_opcode && XGetEventData(display, &event.xcookie)) {
                if (event.xcookie.evtype == XI_RawKeyPress || event.xcookie.evtype == XI_RawKeyRelease) {
                    XIRawEvent *raw_ev = (XIRawEvent*) event.xcookie.data;
                    KeySym keysym = XkbKeycodeToKeysym(display, raw_ev->detail, 0, 0);
                    float direction = (event.xcookie.evtype == XI_RawKeyPress) ? 1.0f : 0.0f;
                    switch (keysym) {
                        case XK_KP_Left:
                        case XK_Left:
                            dx = -direction;
                            break;
                        case XK_KP_Right:
                        case XK_Right:
                            dx = direction;
                            break;
                        case XK_KP_Up:
                        case XK_Up:
                            dy = -direction;
                            break;
                        case XK_KP_Down:
                        case XK_Down:
                            dy = direction;
                            break;
                    }
                }
                XFreeEventData(display, &event.xcookie);
            }
        }
        if (dx != 0 || dy != 0) {
            speed = fmin(speed + acceleration, max_speed);
        } else {
            speed *= deceleration;
            if (speed < base_speed) {
                speed = 0;
                target_x = target_y = 0;
            }
        }
        float move_amount = speed * (frame_time / 1e9f);
        target_x += dx * move_amount;
        target_y += dy * move_amount;
        current_x += (target_x - current_x) * lerp_factor;
        current_y += (target_y - current_y) * lerp_factor;
        if (fabs(current_x) >= 0.5f || fabs(current_y) >= 0.5f) {
            int move_x = roundf(current_x);
            int move_y = roundf(current_y);
            XTestFakeRelativeMotionEvent(display, move_x, move_y, CurrentTime);
            XFlush(display);
            current_x -= move_x;
            current_y -= move_y;
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        elapsed_time = (end_time.tv_sec - start_time.tv_sec) * NANOS_PER_SEC + 
                       (end_time.tv_nsec - start_time.tv_nsec);
        if (elapsed_time < frame_time) {
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = frame_time - elapsed_time;
            nanosleep(&sleep_time, NULL);
        }
    }
}

void inputscan() {
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.xcookie.type == GenericEvent &&
            event.xcookie.extension == xi_opcode &&
            XGetEventData(display, &event.xcookie)) {
            XIRawEvent *raw_ev = (XIRawEvent *)event.xcookie.data;
            if (event.xcookie.evtype == XI_RawKeyPress) {
                KeySym keysym = XkbKeycodeToKeysym(display, raw_ev->detail, 0, 0);
                const char *key_string = XKeysymToString(keysym);
                printf("Key pressed: %u, Character: %s\n", raw_ev->detail, key_string ? key_string : "Unknown");
            } else if (event.xcookie.evtype == XI_RawKeyRelease) {
                KeySym keysym = XkbKeycodeToKeysym(display, raw_ev->detail, 0, 0);
                const char *key_string = XKeysymToString(keysym);
                printf("Key released: %u, Character: %s\n", raw_ev->detail, key_string ? key_string : "Unknown");
            }
            XFreeEventData(display, &event.xcookie);
        }
    }
}
