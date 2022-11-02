/**
 * Copyright (C) 2006 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <glibmm.h>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "ptk/ptk-utils.hxx"

#include "settings.hxx"

void
ptk_show_error(GtkWindow* parent, const std::string& title, const std::string& message)
{
    const std::string msg = Glib::Markup::escape_text(message);
    GtkWidget* dlg = gtk_message_dialog_new(parent,
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_ERROR,
                                            GTK_BUTTONS_OK,
                                            msg.c_str(),
                                            nullptr);
    if (!title.empty())
        gtk_window_set_title(GTK_WINDOW(dlg), title.c_str());
    xset_set_window_icon(GTK_WINDOW(dlg));
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

unsigned int
ptk_get_keymod(unsigned int event)
{
    return (event & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SUPER_MASK |
                     GDK_HYPER_MASK | GDK_META_MASK));
}

GtkBuilder*
_gtk_builder_new_from_file(const char* file)
{
    const std::string filename = Glib::build_filename(PACKAGE_UI_DIR, file);
    GtkBuilder* builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, filename.c_str(), nullptr);

    return builder;
}

#ifdef HAVE_NONLATIN
void
transpose_nonlatin_keypress(GdkEventKey* event)
{
    if (!(event && event->keyval != 0))
        return;

    // is already a latin key?
    if ((GDK_KEY_0 <= event->keyval && event->keyval <= GDK_KEY_9) ||
        (GDK_KEY_A <= event->keyval && event->keyval <= GDK_KEY_Z) ||
        (GDK_KEY_a <= event->keyval && event->keyval <= GDK_KEY_z))
        return;

    // We have a non-latin char, try other keyboard groups
    GdkKeymapKey* keys = nullptr;
    unsigned int* keyvals;
    int n_entries;
    int level;

    GdkDisplay* display = gdk_display_get_default();

    if (gdk_keymap_translate_keyboard_state(gdk_keymap_get_for_display(display),
                                            event->hardware_keycode,
                                            (GdkModifierType)event->state,
                                            event->group,
                                            nullptr,
                                            nullptr,
                                            &level,
                                            nullptr) &&
        gdk_keymap_get_entries_for_keycode(gdk_keymap_get_for_display(display),
                                           event->hardware_keycode,
                                           &keys,
                                           &keyvals,
                                           &n_entries))
    {
        for (int n = 0; n < n_entries; ++n)
        {
            if (keys[n].group == event->group)
                // Skip keys from the same group
                continue;
            if (keys[n].level != level)
                // Allow only same level keys
                continue;
            if ((GDK_KEY_0 <= keyvals[n] && keyvals[n] <= GDK_KEY_9) ||
                (GDK_KEY_A <= keyvals[n] && keyvals[n] <= GDK_KEY_Z) ||
                (GDK_KEY_a <= keyvals[n] && keyvals[n] <= GDK_KEY_z))
            {
                // Latin character found
                event->keyval = keyvals[n];
                break;
            }
        }
        free(keys);
        free(keyvals);
    }
}
#endif
