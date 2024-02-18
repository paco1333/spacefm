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

#pragma once

#include <string_view>

#include <gtkmm.h>

#include "ptk/ptk-file-task.hxx"

#include "main-window.hxx"

enum task_view_column
{
    status,
    count,
    path,
    file,
    to,
    progress,
    total,
    started,
    elapsed,
    curspeed,
    curest,
    avgspeed,
    avgest,
    starttime,
    icon,
    data
};

void main_task_start_queued(GtkWidget* view, ptk::file_task* new_ptask);

void main_task_view_update_task(ptk::file_task* ptask);
void main_task_view_remove_task(ptk::file_task* ptask);
void main_task_pause_all_queued(ptk::file_task* ptask);

void ptk_task_view_task_stop(GtkWidget* view, const xset_t& set2, ptk::file_task* ptask2);

void on_reorder(GtkWidget* item, GtkWidget* parent);

bool ptk_task_view_is_main_tasks_running(GtkWidget* task_view);

void ptk_task_view_prepare_menu(MainWindow* main_window, GtkWidget* menu);

void ptk_task_view_column_selected(GtkWidget* view);

void ptk_task_view_popup_show(MainWindow* main_window, const std::string_view name);
void ptk_task_view_popup_errset(MainWindow* main_window, const std::string_view name = "");

ptk::file_task* ptk_task_view_get_selected_task(GtkWidget* view);

void ptk_task_view_show_task_dialog(GtkWidget* view);

GtkWidget* main_task_view_new(MainWindow* main_window);
