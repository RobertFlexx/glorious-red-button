#include "reasons.hpp"

#include <Application.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Screen.h>
#include <Messenger.h>
#include <View.h>
#include <Window.h>

#include <string>

namespace {

constexpr uint32 done_message = 0x646F6E65; // "done"
const BRect button(105, 125, 345, 197);

class JokeView final : public BView {
public:
    explicit JokeView(BRect frame)
        : BView(frame, "prank", B_FOLLOW_ALL, B_WILL_DRAW), bait_(prank::random_reason()) {
        SetViewColor(255, 255, 255);
    }

    ~JokeView() override { delete timer_; }

    void Draw(BRect) override {
        SetHighColor(255, 255, 255);
        FillRect(Bounds());
        if (revealed_) {
            SetHighColor(230, 57, 70);
            SetFont(be_bold_font);
            SetFontSize(27);
            draw_centered(prank::punchline.data(), 130);
            return;
        }

        SetHighColor(38, 43, 54);
        SetFont(be_plain_font);
        SetFontSize(18);
        draw_centered(bait_.c_str(), 78);

        SetHighColor(230, 57, 70);
        FillRoundRect(button, 12, 12);
        SetHighColor(255, 255, 255);
        SetFont(be_bold_font);
        SetFontSize(20);
        draw_centered("PRESS ME", 161);
    }

    void MouseDown(BPoint where) override {
        if (revealed_ || !button.Contains(where)) return;
        reveal();
    }

    void KeyDown(const char* bytes, int32 count) override {
        if (!revealed_ && count == 1 && (bytes[0] == '\n' || bytes[0] == ' ')) {
            reveal();
            return;
        }
        BView::KeyDown(bytes, count);
    }

    void AttachedToWindow() override { MakeFocus(true); }

private:
    void draw_centered(const char* text, float center_y) {
        font_height height{};
        GetFontHeight(&height);
        DrawString(text, BPoint((prank::window_width - StringWidth(text)) / 2,
            center_y + (height.ascent - height.descent) / 2));
    }

    void reveal() {
        revealed_ = true;
        Invalidate();
        BMessage done(done_message);
        timer_ = new BMessageRunner(BMessenger(Window()), &done,
            static_cast<bigtime_t>(prank::punchline_ms) * 1000, 1);
    }

    std::string bait_;
    BMessageRunner* timer_ = nullptr;
    bool revealed_ = false;
};

class JokeWindow final : public BWindow {
public:
    JokeWindow()
        : BWindow(BRect(100, 100, 100 + prank::window_width, 100 + prank::window_height),
                  "Totally Legit", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE) {
        AddChild(new JokeView(Bounds()));
        BScreen screen(this);
        if (screen.IsValid()) {
            const BRect desktop = screen.Frame();
            MoveTo(desktop.left + (desktop.Width() - Frame().Width()) / 2,
                   desktop.top + (desktop.Height() - Frame().Height()) / 2);
        }
        Show();
    }

    bool QuitRequested() override {
        be_app->PostMessage(B_QUIT_REQUESTED);
        return true;
    }

    void MessageReceived(BMessage* message) override {
        if (message->what == done_message) {
            be_app->PostMessage(B_QUIT_REQUESTED);
            return;
        }
        BWindow::MessageReceived(message);
    }
};

} // namespace

int main() {
    BApplication app("application/x-vnd.big-red-button");
    new JokeWindow();
    app.Run();
    return 0;
}
