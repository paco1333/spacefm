/**
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

#include <string>
#include <string_view>

#include <format>

#include <span>
#include <vector>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "settings/app.hxx"

#include "ptk/ptk-file-task.hxx"

#include "ptk/ptk-file-actions-misc.hxx"

static bool
action_dialog(GtkWindow* parent_win, const std::string_view title, const std::string_view message)
{
    GtkWidget* dlg = gtk_message_dialog_new(parent_win,
                                            GtkDialogFlags::GTK_DIALOG_MODAL,
                                            GtkMessageType::GTK_MESSAGE_WARNING,
                                            GtkButtonsType::GTK_BUTTONS_YES_NO,
                                            message.data(),
                                            nullptr);
    gtk_dialog_set_default_response(GTK_DIALOG(dlg), GtkResponseType::GTK_RESPONSE_YES);
    gtk_window_set_title(GTK_WINDOW(dlg), title.data());
    // xset_set_window_icon(GTK_WINDOW(dlg));

    const i32 response = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    if (response != GtkResponseType::GTK_RESPONSE_YES)
    {
        return false;
    }
    return true;
}

void
ptk_delete_files(GtkWindow* parent_win, const std::filesystem::path& cwd,
                 const std::span<const vfs::file_info> sel_files, GtkTreeView* task_view)
{
    if (sel_files.empty())
    {
        return;
    }

    if (app_settings.get_confirm_delete())
    {
        const std::string msg = std::format("Delete {} selected item ?", sel_files.size());
        const bool ret = action_dialog(parent_win, "Confirm Delete", msg);
        if (!ret)
        {
            return;
        }
    }

    std::vector<std::filesystem::path> file_list;
    file_list.reserve(sel_files.size());
    for (vfs::file_info file : sel_files)
    {
        const auto file_path = cwd / file->get_name();
        file_list.emplace_back(file_path);
    }

    PtkFileTask* ptask = ptk_file_task_new(vfs::file_task_type::DELETE,
                                           file_list,
                                           parent_win ? GTK_WINDOW(parent_win) : nullptr,
                                           GTK_WIDGET(task_view));
    ptk_file_task_run(ptask);
}

void
ptk_trash_files(GtkWindow* parent_win, const std::filesystem::path& cwd,
                const std::span<const vfs::file_info> sel_files, GtkTreeView* task_view)
{
    if (sel_files.empty())
    {
        return;
    }

    if (app_settings.get_confirm_trash())
    {
        const std::string msg = std::format("Trash {} selected item ?", sel_files.size());
        const bool ret = action_dialog(parent_win, "Confirm Trash", msg);
        if (!ret)
        {
            return;
        }
    }

    std::vector<std::filesystem::path> file_list;
    file_list.reserve(sel_files.size());
    for (vfs::file_info file : sel_files)
    {
        const auto file_path = cwd / file->get_name();
        file_list.emplace_back(file_path);
    }

    PtkFileTask* ptask = ptk_file_task_new(vfs::file_task_type::trash,
                                           file_list,
                                           parent_win ? GTK_WINDOW(parent_win) : nullptr,
                                           GTK_WIDGET(task_view));
    ptk_file_task_run(ptask);
}
