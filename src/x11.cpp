#include "reasons.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <poll.h>
#include <string>

namespace {

constexpr int button_x = 105;
constexpr int button_y = 125;
constexpr int button_w = 240;
constexpr int button_h = 72;

unsigned long color(Display* display, Colormap map, unsigned short r,
                    unsigned short g, unsigned short b, unsigned long fallback) {
    XColor value{};
    value.red = r;
    value.green = g;
    value.blue = b;
    value.flags = DoRed | DoGreen | DoBlue;
    return XAllocColor(display, map, &value) ? value.pixel : fallback;
}

XFontStruct* load_font(Display* display, const char* preferred, const char* backup) {
    if (auto* font = XLoadQueryFont(display, preferred)) return font;
    if (auto* font = XLoadQueryFont(display, backup)) return font;
    return XLoadQueryFont(display, "fixed");
}

class App {
public:
    bool start() {
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            std::fputs("redbutton: cannot connect to X11; install GTK3 for native Wayland.\n", stderr);
            return false;
        }

        const int screen = DefaultScreen(display_);
        const auto black = BlackPixel(display_, screen);
        const auto white = WhitePixel(display_, screen);
        const auto map = DefaultColormap(display_, screen);
        background_ = white;
        ink_ = color(display_, map, 0x2424, 0x2929, 0x3535, black);
        red_ = color(display_, map, 0xe6e6, 0x3939, 0x4646, black);
        dark_red_ = color(display_, map, 0xbcbc, 0x2626, 0x3232, black);
        white_ = white;
        bait_ = prank::random_reason();

        const int left = (DisplayWidth(display_, screen) - prank::window_width) / 2;
        const int top = (DisplayHeight(display_, screen) - prank::window_height) / 2;
        window_ = XCreateSimpleWindow(display_, RootWindow(display_, screen),
                                      left, top, prank::window_width,
                                      prank::window_height, 0, black, white);
        if (!window_) {
            XCloseDisplay(display_);
            display_ = nullptr;
            return false;
        }

        XStoreName(display_, window_, "Totally Legit");
        XSelectInput(display_, window_, ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask);
        Atom name = XInternAtom(display_, "WM_DELETE_WINDOW", False);
        close_atom_ = name;
        XSetWMProtocols(display_, window_, &name, 1);

        XSizeHints hints{};
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = hints.max_width = prank::window_width;
        hints.min_height = hints.max_height = prank::window_height;
        XSetWMNormalHints(display_, window_, &hints);

        gc_ = XCreateGC(display_, window_, 0, nullptr);
        heading_font_ = load_font(display_, "-*-helvetica-medium-r-normal--18-*-*-*-*-*-*-*", "9x15");
        bold_font_ = load_font(display_, "-*-helvetica-bold-r-normal--20-*-*-*-*-*-*-*", "10x20");
        if (!heading_font_ || !bold_font_) {
            std::fputs("redbutton: X11 server has no usable fonts.\n", stderr);
            cleanup();
            return false;
        }

        XMapRaised(display_, window_);
        XFlush(display_);
        return true;
    }

    void run() {
        bool running = true;
        while (running) {
            if (revealed_ && std::chrono::steady_clock::now() >= deadline_) break;

            while (XPending(display_) > 0) {
                XEvent event{};
                XNextEvent(display_, &event);
                switch (event.type) {
                case Expose:
                    if (event.xexpose.count == 0) draw();
                    break;
                case ButtonPress:
                    if (!revealed_ && event.xbutton.button == Button1 &&
                        event.xbutton.x >= button_x && event.xbutton.x < button_x + button_w &&
                        event.xbutton.y >= button_y && event.xbutton.y < button_y + button_h) {
                        reveal();
                    }
                    break;
                case KeyPress: {
                    const auto key = XLookupKeysym(&event.xkey, 0);
                    if (!revealed_ && (key == XK_Return || key == XK_KP_Enter || key == XK_space)) reveal();
                    if (key == XK_Escape) running = false;
                    break;
                }
                case ClientMessage:
                    if (static_cast<Atom>(event.xclient.data.l[0]) == close_atom_) running = false;
                    break;
                case DestroyNotify:
                    running = false;
                    break;
                }
                if (!running) break;
            }
            if (!running || (revealed_ && std::chrono::steady_clock::now() >= deadline_)) break;

            int timeout = -1;
            if (revealed_) {
                const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                    deadline_ - std::chrono::steady_clock::now()).count();
                timeout = remaining > 0 ? static_cast<int>(remaining) + 1 : 0;
            }
            pollfd fd{ConnectionNumber(display_), POLLIN, 0};
            if (poll(&fd, 1, timeout) < 0 && errno != EINTR) {
                std::fprintf(stderr, "redbutton: poll: %s\n", std::strerror(errno));
                break;
            }
        }
        cleanup();
    }

private:
    void centered_text(const char* text, int y, XFontStruct* font, unsigned long shade) {
        XSetFont(display_, gc_, font->fid);
        XSetForeground(display_, gc_, shade);
        const int width = XTextWidth(font, text, static_cast<int>(std::strlen(text)));
        const int baseline = y + (font->ascent - font->descent) / 2;
        XDrawString(display_, window_, gc_, (prank::window_width - width) / 2,
                    baseline, text, static_cast<int>(std::strlen(text)));
    }

    void draw() {
        XSetForeground(display_, gc_, background_);
        XFillRectangle(display_, window_, gc_, 0, 0, prank::window_width, prank::window_height);
        if (revealed_) {
            centered_text(prank::punchline.data(), 130, bold_font_, red_);
        } else {
            centered_text(bait_.c_str(), 77, heading_font_, ink_);
            XSetForeground(display_, gc_, dark_red_);
            XFillRectangle(display_, window_, gc_, button_x - 1, button_y - 1,
                           button_w + 2, button_h + 2);
            XSetForeground(display_, gc_, red_);
            XFillRectangle(display_, window_, gc_, button_x, button_y, button_w, button_h);
            centered_text("PRESS ME", button_y + button_h / 2, bold_font_, white_);
        }
        XFlush(display_);
    }

    void reveal() {
        revealed_ = true;
        deadline_ = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(prank::punchline_ms);
        draw();
    }

    void cleanup() {
        if (!display_) return;
        if (heading_font_) XFreeFont(display_, heading_font_);
        if (bold_font_) XFreeFont(display_, bold_font_);
        if (gc_) XFreeGC(display_, gc_);
        if (window_) XDestroyWindow(display_, window_);
        XCloseDisplay(display_);
        display_ = nullptr;
    }

    Display* display_ = nullptr;
    Window window_ = 0;
    GC gc_ = nullptr;
    Atom close_atom_ = None;
    XFontStruct* heading_font_ = nullptr;
    XFontStruct* bold_font_ = nullptr;
    unsigned long background_ = 0, ink_ = 0, red_ = 0, dark_red_ = 0, white_ = 0;
    std::string bait_;
    bool revealed_ = false;
    std::chrono::steady_clock::time_point deadline_{};
};

} // namespace

int main() {
    App app;
    if (!app.start()) return 1;
    app.run();
    return 0;
}
