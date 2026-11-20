#include "reasons.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

namespace {

constexpr int button_id = 100;
constexpr UINT_PTR close_timer = 1;

HWND bait_label = nullptr;
HWND red_button = nullptr;
HWND result_label = nullptr;
HFONT regular_font = nullptr;
HFONT bold_font = nullptr;
bool revealed = false;

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_CREATE: {
        const std::string reason = prank::random_reason();
        const std::wstring bait(reason.begin(), reason.end());
        const std::string punchline(prank::punchline);
        const std::wstring result(punchline.begin(), punchline.end());
        regular_font = CreateFontW(-21, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        bold_font = CreateFontW(-27, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        bait_label = CreateWindowExW(0, L"STATIC", bait.c_str(),
            WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            20, 48, 410, 44, window, nullptr, nullptr, nullptr);
        red_button = CreateWindowExW(0, L"BUTTON", L"PRESS ME",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
            105, 125, 240, 72, window,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(button_id)), nullptr, nullptr);
        result_label = CreateWindowExW(0, L"STATIC", result.c_str(),
            WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            20, 96, 410, 72, window, nullptr, nullptr, nullptr);
        SendMessageW(bait_label, WM_SETFONT, reinterpret_cast<WPARAM>(regular_font), TRUE);
        SendMessageW(red_button, WM_SETFONT, reinterpret_cast<WPARAM>(regular_font), TRUE);
        SendMessageW(result_label, WM_SETFONT, reinterpret_cast<WPARAM>(bold_font), TRUE);
        return 0;
    }
    case WM_DRAWITEM: {
        auto* item = reinterpret_cast<DRAWITEMSTRUCT*>(lparam);
        if (item->CtlID != button_id) break;
        HDC dc = item->hDC;
        RECT r = item->rcItem;
        HBRUSH brush = CreateSolidBrush((item->itemState & ODS_SELECTED)
            ? RGB(174, 37, 51) : RGB(230, 57, 70));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(192, 43, 55));
        HGDIOBJ previous_brush = SelectObject(dc, brush);
        HGDIOBJ previous_pen = SelectObject(dc, pen);
        RoundRect(dc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1, 16, 16);
        SelectObject(dc, previous_brush);
        SelectObject(dc, previous_pen);
        DeleteObject(brush);
        DeleteObject(pen);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        HGDIOBJ previous_font = SelectObject(dc, bold_font);
        DrawTextW(dc, L"PRESS ME", -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(dc, previous_font);
        if (item->itemState & ODS_FOCUS) {
            InflateRect(&r, -5, -5);
            DrawFocusRect(dc, &r);
        }
        return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wparam) == button_id && HIWORD(wparam) == BN_CLICKED && !revealed) {
            revealed = true;
            ShowWindow(bait_label, SW_HIDE);
            ShowWindow(red_button, SW_HIDE);
            ShowWindow(result_label, SW_SHOW);
            InvalidateRect(window, nullptr, TRUE);
            SetTimer(window, close_timer, prank::punchline_ms, nullptr);
            return 0;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HDC dc = reinterpret_cast<HDC>(wparam);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, reinterpret_cast<HWND>(lparam) == result_label
            ? RGB(230, 57, 70) : RGB(38, 43, 54));
        return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
    }
    case WM_TIMER:
        if (wparam == close_timer) {
            KillTimer(window, close_timer);
            DestroyWindow(window);
            return 0;
        }
        break;
    case WM_DESTROY:
        DeleteObject(regular_font);
        DeleteObject(bold_font);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show_command) {
    SetProcessDPIAware();
    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = L"BigRedButtonWindow";
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    if (!RegisterClassExW(&window_class)) return 1;

    RECT rect{0, 0, prank::window_width, prank::window_height};
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    const int top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    HWND window = CreateWindowExW(0, window_class.lpszClassName, L"Totally Legit",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        left, top, width, height, nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    ShowWindow(window, show_command);
    UpdateWindow(window);
    SetFocus(red_button);

    MSG event{};
    while (GetMessageW(&event, nullptr, 0, 0) > 0) {
        TranslateMessage(&event);
        DispatchMessageW(&event);
    }
    return static_cast<int>(event.wParam);
}
