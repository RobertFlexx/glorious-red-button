#include "reasons.hpp"

#include <gtk/gtk.h>

#include <string>

namespace {

struct Widgets {
    GtkWidget* bait = nullptr;
    GtkWidget* button = nullptr;
    GtkWidget* result = nullptr;
    guint timer_id = 0;
    bool revealed = false;
};

gboolean close_after_joke(gpointer) {
    gtk_main_quit();
    return G_SOURCE_REMOVE;
}

void on_destroy(GtkWidget*, gpointer user_data) {
    auto& ui = *static_cast<Widgets*>(user_data);
    if (ui.timer_id != 0) {
        g_source_remove(ui.timer_id);
        ui.timer_id = 0;
    }
    gtk_main_quit();
}

void clicked(GtkButton*, gpointer user_data) {
    auto& ui = *static_cast<Widgets*>(user_data);
    if (ui.revealed) return;
    ui.revealed = true;
    gtk_widget_hide(ui.bait);
    gtk_widget_hide(ui.button);
    gtk_widget_show(ui.result);
    ui.timer_id = g_timeout_add(prank::punchline_ms, close_after_joke, nullptr);
}

void apply_style() {
    constexpr auto css = R"CSS(
        /* Wayland uses client-side decorations and a theme-provided
           background; force the same white canvas every other backend has. */
        window.background, #joke-box {
            background-color: #ffffff;
            background-image: none;
            color: #262b36;
        }
        #joke-prompt { color: #262b36; font-size: 18px; }
        #joke-result { color: #e63946; font-size: 26px; font-weight: bold; }
        #joke-button {
            background-image: none; background-color: #e63946;
            color: white; border: 0; border-radius: 12px;
            font-size: 20px; font-weight: bold;
            box-shadow: none; text-shadow: none;
        }
        #joke-button:hover { background-color: #ce3140; }
        #joke-button:active { background-color: #ac2633; }
    )CSS";
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, nullptr);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

} // namespace

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    apply_style();
    const std::string bait = prank::random_reason();
    Widgets ui;

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Totally Legit");
    gtk_window_set_default_size(GTK_WINDOW(window), prank::window_width, prank::window_height);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), &ui);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(box, "joke-box");
    gtk_container_set_border_width(GTK_CONTAINER(box), 30);
    gtk_container_add(GTK_CONTAINER(window), box);

    ui.bait = gtk_label_new(bait.c_str());
    gtk_widget_set_name(ui.bait, "joke-prompt");
    gtk_widget_set_halign(ui.bait, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), ui.bait, FALSE, FALSE, 20);

    ui.button = gtk_button_new_with_label("PRESS ME");
    gtk_widget_set_name(ui.button, "joke-button");
    gtk_widget_set_size_request(ui.button, 240, 72);
    gtk_widget_set_halign(ui.button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(ui.button, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), ui.button, TRUE, TRUE, 0);
    g_signal_connect(ui.button, "clicked", G_CALLBACK(clicked), &ui);

    ui.result = gtk_label_new(prank::punchline);
    gtk_widget_set_name(ui.result, "joke-result");
    gtk_widget_set_halign(ui.result, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(ui.result, GTK_ALIGN_CENTER);
    gtk_widget_set_no_show_all(ui.result, TRUE);
    gtk_box_pack_start(GTK_BOX(box), ui.result, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_widget_grab_focus(ui.button);
    gtk_main();
    return 0;
}
