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

#include <array>
#include <vector>

#include <chrono>

#include <sys/wait.h>

#include <fmt/format.h>

#include <glibmm.h>
#include <glibmm/convert.h>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "settings/app.hxx"

#include "main-window.hxx"
#include "utils.hxx"

#include "vfs/vfs-utils.hxx"

#include "ptk/ptk-file-task.hxx"

static bool on_vfs_file_task_state_cb(VFSFileTask* task, VFSFileTaskState state, void* state_data,
                                      void* user_data);

static void query_overwrite(PtkFileTask* ptask);

static void ptk_file_task_update(PtkFileTask* ptask);
static bool ptk_file_task_add_main(PtkFileTask* ptask);
static void on_progress_dlg_response(GtkDialog* dlg, int response, PtkFileTask* ptask);
static void save_progress_dialog_size(PtkFileTask* ptask);

PtkFileTask::PtkFileTask(VFSFileTaskType type, const std::vector<std::string>& src_files,
                         const char* dest_dir, GtkWindow* parent_window, GtkWidget* task_view)
{
    this->task = vfs_task_new(type, src_files, dest_dir);

    vfs_file_task_set_state_callback(this->task, on_vfs_file_task_state_cb, this);
    this->parent_window = parent_window;
    this->task_view = task_view;
    this->task->exec_ptask = (void*)this;
    this->progress_dlg = nullptr;
    this->complete = false;
    this->aborted = false;
    this->pause_change = false;
    this->pause_change_view = true;
    this->force_scroll = false;
    this->keep_dlg = false;
    this->err_count = 0;
    if (xset_get_b(XSetName::TASK_ERR_ANY))
        this->err_mode = PTKFileTaskPtaskError::PTASK_ERROR_ANY;
    else if (xset_get_b(XSetName::TASK_ERR_FIRST))
        this->err_mode = PTKFileTaskPtaskError::PTASK_ERROR_FIRST;
    else
        this->err_mode = PTKFileTaskPtaskError::PTASK_ERROR_CONT;

    GtkTextIter iter;
    this->log_buf = gtk_text_buffer_new(nullptr);
    this->log_end = gtk_text_mark_new(nullptr, false);
    gtk_text_buffer_get_end_iter(this->log_buf, &iter);
    gtk_text_buffer_add_mark(this->log_buf, this->log_end, &iter);
    this->log_appended = false;
    this->restart_timeout = false;

    this->complete_notify = nullptr;
    this->user_data = nullptr;

    this->progress_count = 0;
    this->pop_handler = nullptr;

    this->query_cond = nullptr;
    this->query_cond_last = nullptr;
    this->query_new_dest = nullptr;

    // queue task
    if (this->task->exec_sync && this->task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC &&
        this->task->type != VFSFileTaskType::VFS_FILE_TASK_LINK &&
        this->task->type != VFSFileTaskType::VFS_FILE_TASK_CHMOD_CHOWN &&
        xset_get_b(XSetName::TASK_Q_NEW))
        ptk_file_task_pause(this, VFSFileTaskState::VFS_FILE_TASK_QUEUE);

    // GThread *self = g_thread_self ();
    // LOG_INFO("GUI_THREAD = {:p}", fmt::ptr(self));
    // LOG_INFO("ptk_file_task_new DONE ptask={:p}", fmt::ptr(this));
}

PtkFileTask::~PtkFileTask()
{
    // LOG_INFO("ptk_file_task_destroy ptask={:p}", fmt::ptr(ptask));
    if (this->timeout)
    {
        g_source_remove(this->timeout);
        this->timeout = 0;
    }
    if (this->progress_timer)
    {
        g_source_remove(this->progress_timer);
        this->progress_timer = 0;
    }
    main_task_view_remove_task(this);
    main_task_start_queued(this->task_view, nullptr);

    if (this->progress_dlg)
    {
        save_progress_dialog_size(this);

        if (this->overwrite_combo)
            gtk_combo_box_popdown(GTK_COMBO_BOX(this->overwrite_combo));
        if (this->error_combo)
            gtk_combo_box_popdown(GTK_COMBO_BOX(this->error_combo));
        gtk_widget_destroy(this->progress_dlg);
        this->progress_dlg = nullptr;
    }
    if (this->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        // LOG_INFO("    g_io_channel_shutdown");
        // channel shutdowns are needed to stop channel reads after task ends.
        // Cannot be placed in cb_exec_child_watch because it causes single
        // line output to be lost
        if (this->task->exec_channel_out)
            g_io_channel_shutdown(this->task->exec_channel_out, true, nullptr);
        if (this->task->exec_channel_err)
            g_io_channel_shutdown(this->task->exec_channel_err, true, nullptr);
        this->task->exec_channel_out = this->task->exec_channel_err = nullptr;
        if (this->task->child_watch)
        {
            g_source_remove(this->task->child_watch);
            this->task->child_watch = 0;
        }
        // LOG_INFO("    g_io_channel_shutdown DONE");
    }

    if (this->task)
        vfs_file_task_free(this->task);

    gtk_text_buffer_set_text(this->log_buf, "", -1);
    g_object_unref(this->log_buf);

    if (this->pop_handler)
        free(this->pop_handler);

    // LOG_INFO("ptk_file_task_destroy DONE ptask={:p}", fmt::ptr(ptask));
}

void
ptk_file_task_lock(PtkFileTask* ptask)
{
    g_mutex_lock(ptask->task->mutex);
}

void
ptk_file_task_unlock(PtkFileTask* ptask)
{
    g_mutex_unlock(ptask->task->mutex);
}

static bool
ptk_file_task_trylock(PtkFileTask* ptask)
{
    return g_mutex_trylock(ptask->task->mutex);
}

PtkFileTask*
ptk_file_exec_new(std::string_view item_name, const char* dir, GtkWidget* parent,
                  GtkWidget* task_view)
{
    GtkWidget* parent_win = nullptr;
    if (parent)
        parent_win = gtk_widget_get_toplevel(GTK_WIDGET(parent));

    const std::vector<std::string> file_list{item_name.data()};
    PtkFileTask* ptask = new PtkFileTask(VFSFileTaskType::VFS_FILE_TASK_EXEC,
                                         file_list,
                                         dir,
                                         GTK_WINDOW(parent_win),
                                         task_view);

    return ptask;
}

static void
save_progress_dialog_size(PtkFileTask* ptask)
{
    // save dialog size  - do this here now because as of GTK 3.8,
    // allocation == 1,1 in destroy event
    GtkAllocation allocation;

    gtk_widget_get_allocation(GTK_WIDGET(ptask->progress_dlg), &allocation);

    const std::string width = std::to_string(allocation.width);
    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
        xset_set(XSetName::TASK_POP_TOP, XSetVar::S, width);
    else
        xset_set(XSetName::TASK_POP_TOP, XSetVar::X, width);

    const std::string height = std::to_string(allocation.height);
    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
        xset_set(XSetName::TASK_POP_TOP, XSetVar::Z, height);
    else
        xset_set(XSetName::TASK_POP_TOP, XSetVar::Y, height);
}

void
ptk_file_task_set_complete_notify(PtkFileTask* ptask, GFunc callback, void* user_data)
{
    ptask->complete_notify = callback;
    ptask->user_data = user_data;
}

static bool
on_progress_timer(PtkFileTask* ptask)
{
    // GThread *self = g_thread_self ();
    // LOG_INFO("PROGRESS_TIMER_THREAD = {:p}", fmt::ptr(self));

    // query condition?
    if (ptask->query_cond && ptask->query_cond != ptask->query_cond_last)
    {
        // LOG_INFO("QUERY = {:p}  mutex = {:p}", fmt::ptr(ptask->query_cond),
        // fmt::ptr(ptask->task->mutex));
        ptask->restart_timeout = (ptask->timeout != 0);
        if (ptask->timeout)
        {
            g_source_remove(ptask->timeout);
            ptask->timeout = 0;
        }
        if (ptask->progress_timer)
        {
            g_source_remove(ptask->progress_timer);
            ptask->progress_timer = 0;
        }

        ptk_file_task_lock(ptask);
        query_overwrite(ptask);
        ptk_file_task_unlock(ptask);
        return false;
    }

    // start new queued task
    if (ptask->task->queue_start)
    {
        ptask->task->queue_start = false;
        if (ptask->task->state_pause == VFSFileTaskState::VFS_FILE_TASK_RUNNING)
            ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_RUNNING);
        else
            main_task_start_queued(ptask->task_view, ptask);
        if (ptask->timeout && ptask->task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING &&
            ptask->task->state == VFSFileTaskState::VFS_FILE_TASK_RUNNING)
        {
            // task is waiting in queue so list it
            g_source_remove(ptask->timeout);
            ptask->timeout = 0;
        }
    }

    // only update every 300ms (6 * 50ms)
    if (++ptask->progress_count < 6)
        return true;
    ptask->progress_count = 0;
    // LOG_INFO("on_progress_timer ptask={:p}", ptask);

    if (ptask->complete)
    {
        if (ptask->progress_timer)
        {
            g_source_remove(ptask->progress_timer);
            ptask->progress_timer = 0;
        }
        if (ptask->complete_notify)
        {
            ptask->complete_notify(ptask->task, ptask->user_data);
            ptask->complete_notify = nullptr;
        }
        main_task_view_remove_task(ptask);
        main_task_start_queued(ptask->task_view, nullptr);
    }
    else if (ptask->task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING &&
             !ptask->pause_change && ptask->task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
        return true;

    ptk_file_task_update(ptask);

    if (ptask->complete)
    {
        if (!ptask->progress_dlg || (!ptask->err_count && !ptask->keep_dlg))
        {
            delete ptask;
            // LOG_INFO("on_progress_timer DONE false-COMPLETE ptask={:p}", ptask);
            return false;
        }
        else if (ptask->progress_dlg && ptask->err_count)
            gtk_window_present(GTK_WINDOW(ptask->progress_dlg));
    }
    // LOG_INFO("on_progress_timer DONE true ptask={:p}", ptask);
    return !ptask->complete;
}

static bool
ptk_file_task_add_main(PtkFileTask* ptask)
{
    // LOG_INFO("ptk_file_task_add_main ptask={:p}", fmt::ptr(ptask));
    if (ptask->timeout)
    {
        g_source_remove(ptask->timeout);
        ptask->timeout = 0;
    }

    if (ptask->task->exec_popup || xset_get_b(XSetName::TASK_POP_ALL))
    {
        // keep dlg if Popup Task is explicitly checked, otherwise close if no
        // error
        ptask->keep_dlg = ptask->keep_dlg || ptask->task->exec_popup;
        ptk_file_task_progress_open(ptask);
    }

    if (ptask->task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING && !ptask->pause_change)
        ptask->pause_change = ptask->pause_change_view = true;

    on_progress_timer(ptask);

    // LOG_INFO("ptk_file_task_add_main DONE ptask={:p}", fmt::ptr(ptask));
    return false;
}

void
ptk_file_task_run(PtkFileTask* ptask)
{
    // LOG_INFO("ptk_file_task_run ptask={:p}", fmt::ptr(ptask));
    // wait this long to first show task in manager, popup
    ptask->timeout = g_timeout_add(500, (GSourceFunc)ptk_file_task_add_main, ptask);
    ptask->progress_timer = 0;
    vfs_file_task_run(ptask->task);
    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        if ((ptask->complete || !ptask->task->exec_sync) && ptask->timeout)
        {
            g_source_remove(ptask->timeout);
            ptask->timeout = 0;
        }
    }
    ptask->progress_timer = g_timeout_add(50, (GSourceFunc)on_progress_timer, ptask);
    // LOG_INFO("ptk_file_task_run DONE ptask={:p}", fmt::ptr(ptask));
}

static bool
ptk_file_task_kill(void* pid)
{
    // LOG_INFO("SIGKILL {}", GPOINTER_TO_INT(pid));
    kill(GPOINTER_TO_INT(pid), SIGKILL);
    return false;
}

static bool
ptk_file_task_kill_cpids(char* cpids)
{
    vfs_file_task_kill_cpids(cpids, SIGKILL);
    free(cpids);
    return false;
}

bool
ptk_file_task_cancel(PtkFileTask* ptask)
{
    // GThread *self = g_thread_self ();
    // LOG_INFO("CANCEL_THREAD = {:p}", fmt::ptr(self));
    if (ptask->timeout)
    {
        g_source_remove(ptask->timeout);
        ptask->timeout = 0;
    }
    ptask->aborted = true;
    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        ptask->keep_dlg = true;

        // resume task for task list responsiveness
        if (ptask->task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING)
            ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_RUNNING);

        vfs_file_task_abort(ptask->task);

        if (ptask->task->exec_pid)
        {
            // LOG_INFO("SIGTERM {}", ptask->task->exec_pid);
            char* cpids = vfs_file_task_get_cpids(ptask->task->exec_pid);
            kill(ptask->task->exec_pid, SIGTERM);
            if (cpids)
                vfs_file_task_kill_cpids(cpids, SIGTERM);
            // SIGKILL 2 seconds later in case SIGTERM fails
            g_timeout_add(2500,
                          (GSourceFunc)ptk_file_task_kill,
                          GINT_TO_POINTER(ptask->task->exec_pid));
            if (cpids)
                g_timeout_add(2500, (GSourceFunc)ptk_file_task_kill_cpids, cpids);
        }
        else
        {
            // no pid (exited)
            // user pressed Stop on an exited process, remove task
            // this may be needed because if process is killed, channels may not
            // receive HUP and may remain open, leaving the task listed
            ptask->complete = true;
        }

        if (ptask->task->exec_cond)
        {
            // this is used only if exec task run in non-main loop thread
            ptk_file_task_lock(ptask);
            if (ptask->task->exec_cond)
                g_cond_broadcast(ptask->task->exec_cond);
            ptk_file_task_unlock(ptask);
        }
    }
    else
        vfs_file_task_try_abort(ptask->task);
    return false;
}

static void
set_button_states(PtkFileTask* ptask)
{
    // const char* icon;
    // const char* iconset;
    const char* label;
    bool sens = !ptask->complete;

    if (!ptask->progress_dlg)
        return;

    switch (ptask->task->state_pause)
    {
        case VFSFileTaskState::VFS_FILE_TASK_PAUSE:
            label = "Q_ueue";
            // iconset = ztd::strdup("task_que");
            //  icon = "list-add";
            break;
        case VFSFileTaskState::VFS_FILE_TASK_QUEUE:
            label = "Res_ume";
            // iconset = ztd::strdup("task_resume");
            //  icon = "media-playback-start";
            break;
        case VFSFileTaskState::VFS_FILE_TASK_RUNNING:
        case VFSFileTaskState::VFS_FILE_TASK_SIZE_TIMEOUT:
        case VFSFileTaskState::VFS_FILE_TASK_QUERY_OVERWRITE:
        case VFSFileTaskState::VFS_FILE_TASK_ERROR:
        case VFSFileTaskState::VFS_FILE_TASK_FINISH:
        default:
            label = "Pa_use";
            // iconset = ztd::strdup("task_pause");
            //  icon = "media-playback-pause";
            break;
    }
    sens = sens &&
           !(ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC && !ptask->task->exec_pid);

    /*
    xset_t set = xset_get(iconset);
    if (set->icon)
        icon = set->icon;
    */

    gtk_widget_set_sensitive(ptask->progress_btn_pause, sens);
    gtk_button_set_label(GTK_BUTTON(ptask->progress_btn_pause), label);
    gtk_widget_set_sensitive(ptask->progress_btn_close, ptask->complete || !!ptask->task_view);
}

void
ptk_file_task_pause(PtkFileTask* ptask, int state)
{
    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        // exec task
        // ptask->keep_dlg = true;
        int sig;
        if (state == VFSFileTaskState::VFS_FILE_TASK_PAUSE ||
            (ptask->task->state_pause == VFSFileTaskState::VFS_FILE_TASK_RUNNING &&
             state == VFSFileTaskState::VFS_FILE_TASK_QUEUE))
        {
            sig = SIGSTOP;
            ptask->task->state_pause = (VFSFileTaskState)state;
            ptask->task->timer.stop();
        }
        else if (state == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
        {
            sig = 0;
            ptask->task->state_pause = (VFSFileTaskState)state;
        }
        else
        {
            sig = SIGCONT;
            ptask->task->state_pause = VFSFileTaskState::VFS_FILE_TASK_RUNNING;
            ptask->task->timer.start();
        }

        if (sig && ptask->task->exec_pid)
        {
            // send signal
            char* cpids = vfs_file_task_get_cpids(ptask->task->exec_pid);

            kill(ptask->task->exec_pid, sig);
            if (cpids)
                vfs_file_task_kill_cpids(cpids, sig);
        }
    }
    else if (state == VFSFileTaskState::VFS_FILE_TASK_PAUSE)
        ptask->task->state_pause = VFSFileTaskState::VFS_FILE_TASK_PAUSE;
    else if (state == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
        ptask->task->state_pause = VFSFileTaskState::VFS_FILE_TASK_QUEUE;
    else
    {
        // Resume
        if (ptask->task->pause_cond)
        {
            ptk_file_task_lock(ptask);
            g_cond_broadcast(ptask->task->pause_cond);
            ptk_file_task_unlock(ptask);
        }
        ptask->task->state_pause = VFSFileTaskState::VFS_FILE_TASK_RUNNING;
    }
    set_button_states(ptask);
    ptask->pause_change = ptask->pause_change_view = true;
    ptask->progress_count = 50; // trigger fast display
}

static bool
on_progress_dlg_delete_event(GtkWidget* widget, GdkEvent* event, PtkFileTask* ptask)
{
    (void)widget;
    (void)event;
    save_progress_dialog_size(ptask);
    return !(ptask->complete || ptask->task_view);
}

static void
on_progress_dlg_response(GtkDialog* dlg, int response, PtkFileTask* ptask)
{
    (void)dlg;
    save_progress_dialog_size(ptask);
    if (ptask->complete && !ptask->complete_notify)
    {
        delete ptask;
        return;
    }
    switch (response)
    {
        case GTK_RESPONSE_CANCEL: // Stop btn
            ptask->keep_dlg = false;
            if (ptask->overwrite_combo)
                gtk_combo_box_popdown(GTK_COMBO_BOX(ptask->overwrite_combo));
            if (ptask->error_combo)
                gtk_combo_box_popdown(GTK_COMBO_BOX(ptask->error_combo));
            gtk_widget_destroy(ptask->progress_dlg);
            ptask->progress_dlg = nullptr;
            ptk_file_task_cancel(ptask);
            break;
        case GTK_RESPONSE_NO: // Pause btn
            if (ptask->task->state_pause == VFSFileTaskState::VFS_FILE_TASK_PAUSE)
            {
                ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_QUEUE);
            }
            else if (ptask->task->state_pause == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
            {
                ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_RUNNING);
            }
            else
            {
                ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_PAUSE);
            }
            main_task_start_queued(ptask->task_view, nullptr);
            break;
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_NONE:
            ptask->keep_dlg = false;
            if (ptask->overwrite_combo)
                gtk_combo_box_popdown(GTK_COMBO_BOX(ptask->overwrite_combo));
            if (ptask->error_combo)
                gtk_combo_box_popdown(GTK_COMBO_BOX(ptask->error_combo));
            gtk_widget_destroy(ptask->progress_dlg);
            ptask->progress_dlg = nullptr;
            break;
        default:
            break;
    }
}

static void
on_progress_dlg_destroy(GtkDialog* dlg, PtkFileTask* ptask)
{
    (void)dlg;
    ptask->progress_dlg = nullptr;
}

static void
on_view_popup(GtkTextView* entry, GtkMenu* menu, void* user_data)
{
    (void)entry;
    (void)user_data;
    GtkAccelGroup* accel_group = gtk_accel_group_new();
    xset_context_new();

    xset_t set = xset_get(XSetName::SEPARATOR);
    set->browser = nullptr;
    xset_add_menuitem(nullptr, GTK_WIDGET(menu), accel_group, set);
    gtk_widget_show_all(GTK_WIDGET(menu));
    g_signal_connect(menu, "key-press-event", G_CALLBACK(xset_menu_keypress), nullptr);
}

static void
set_progress_icon(PtkFileTask* ptask)
{
    GdkPixbuf* pixbuf;
    VFSFileTask* task = ptask->task;
    GtkIconTheme* icon_theme = gtk_icon_theme_get_default();

    if (task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING)
        pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                          "media-playback-pause",
                                          16,
                                          GTK_ICON_LOOKUP_USE_BUILTIN,
                                          nullptr);
    else if (task->err_count)
        pixbuf =
            gtk_icon_theme_load_icon(icon_theme, "error", 16, GTK_ICON_LOOKUP_USE_BUILTIN, nullptr);
    else if (task->type == VFSFileTaskType::VFS_FILE_TASK_MOVE ||
             task->type == VFSFileTaskType::VFS_FILE_TASK_COPY ||
             task->type == VFSFileTaskType::VFS_FILE_TASK_LINK ||
             task->type == VFSFileTaskType::VFS_FILE_TASK_TRASH)
        pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                          "stock_copy",
                                          16,
                                          GTK_ICON_LOOKUP_USE_BUILTIN,
                                          nullptr);
    else if (task->type == VFSFileTaskType::VFS_FILE_TASK_DELETE)
        pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                          "stock_delete",
                                          16,
                                          GTK_ICON_LOOKUP_USE_BUILTIN,
                                          nullptr);
    else if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC && !task->exec_icon.empty())
        pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                          task->exec_icon.c_str(),
                                          16,
                                          GTK_ICON_LOOKUP_USE_BUILTIN,
                                          nullptr);
    else
        pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                          "gtk-execute",
                                          16,
                                          GTK_ICON_LOOKUP_USE_BUILTIN,
                                          nullptr);
    gtk_window_set_icon(GTK_WINDOW(ptask->progress_dlg), pixbuf);
}

static void
on_overwrite_combo_changed(GtkComboBox* box, PtkFileTask* ptask)
{
    int overwrite_mode = gtk_combo_box_get_active(box);
    if (overwrite_mode < 0)
        overwrite_mode = 0;
    vfs_file_task_set_overwrite_mode(ptask->task, (VFSFileTaskOverwriteMode)overwrite_mode);
}

static void
on_error_combo_changed(GtkComboBox* box, PtkFileTask* ptask)
{
    int error_mode = gtk_combo_box_get_active(box);
    if (error_mode < 0)
        error_mode = 0;
    ptask->err_mode = PTKFileTaskPtaskError(error_mode);
}

void
ptk_file_task_progress_open(PtkFileTask* ptask)
{
    static constexpr std::array<std::string_view, 7> actions{
        "Move: ",
        "Copy: ",
        "Trash: ",
        "Delete: ",
        "Link: ",
        "Change: ",
        "Run: ",
    };
    static constexpr std::array<std::string_view, 7> titles{
        "Moving...",
        "Copying...",
        "Trashing...",
        "Deleting...",
        "Linking...",
        "Changing...",
        "Running...",
    };

    if (ptask->progress_dlg)
        return;

    // LOG_INFO("ptk_file_task_progress_open");

    VFSFileTask* task = ptask->task;

    ptask->progress_dlg = gtk_dialog_new_with_buttons(titles.at(task->type).data(),
                                                      nullptr /*was task->parent_window*/,
                                                      (GtkDialogFlags)0,
                                                      nullptr,
                                                      nullptr);

    // cache this value for speed
    ptask->pop_detail = xset_get_b(XSetName::TASK_POP_DETAIL);

    // Buttons
    // Pause
    // xset_t set = xset_get(XSetName::TASK_PAUSE);

    ptask->progress_btn_pause = gtk_button_new_with_mnemonic("Pa_use");

    gtk_dialog_add_action_widget(GTK_DIALOG(ptask->progress_dlg),
                                 ptask->progress_btn_pause,
                                 GTK_RESPONSE_NO);
    gtk_widget_set_focus_on_click(GTK_WIDGET(ptask->progress_btn_pause), false);
    // Stop
    ptask->progress_btn_stop = gtk_button_new_with_label("Stop");
    gtk_dialog_add_action_widget(GTK_DIALOG(ptask->progress_dlg),
                                 ptask->progress_btn_stop,
                                 GTK_RESPONSE_CANCEL);
    gtk_widget_set_focus_on_click(GTK_WIDGET(ptask->progress_btn_stop), false);
    // Close
    ptask->progress_btn_close = gtk_button_new_with_label("Close");
    gtk_dialog_add_action_widget(GTK_DIALOG(ptask->progress_dlg),
                                 ptask->progress_btn_close,
                                 GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(ptask->progress_btn_close, !!ptask->task_view);

    set_button_states(ptask);

    GtkGrid* grid = GTK_GRID(gtk_grid_new());

    gtk_container_set_border_width(GTK_CONTAINER(grid), 5);
    gtk_grid_set_row_spacing(grid, 6);
    gtk_grid_set_column_spacing(grid, 4);
    int row = 0;

    /* Copy/Move/Link: */
    GtkLabel* label = GTK_LABEL(gtk_label_new(actions.at(task->type).data()));
    gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_CENTER);
    gtk_grid_attach(grid, GTK_WIDGET(label), 0, row, 1, 1);
    ptask->from = GTK_LABEL(gtk_label_new(ptask->complete ? "" : task->current_file.c_str()));
    gtk_widget_set_halign(GTK_WIDGET(ptask->from), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(ptask->from), GTK_ALIGN_CENTER);
    gtk_label_set_ellipsize(ptask->from, PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_selectable(ptask->from, true);
    gtk_grid_attach(grid, GTK_WIDGET(ptask->from), 1, row, 1, 1);

    if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        // From: <src directory>
        row++;
        label = GTK_LABEL(gtk_label_new("From:"));
        gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_CENTER);
        gtk_grid_attach(grid, GTK_WIDGET(label), 0, row, 1, 1);
        ptask->src_dir = GTK_LABEL(gtk_label_new(nullptr));
        gtk_widget_set_halign(GTK_WIDGET(ptask->src_dir), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(ptask->src_dir), GTK_ALIGN_CENTER);
        gtk_label_set_ellipsize(ptask->src_dir, PANGO_ELLIPSIZE_MIDDLE);
        gtk_label_set_selectable(ptask->src_dir, true);
        gtk_grid_attach(grid, GTK_WIDGET(ptask->src_dir), 1, row, 1, 1);
        if (!task->dest_dir.empty())
        {
            /* To: <Destination directory>
            ex. Copy file to..., Move file to...etc. */
            row++;
            label = GTK_LABEL(gtk_label_new("To:"));
            gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_CENTER);
            gtk_grid_attach(grid, GTK_WIDGET(label), 0, row, 1, 1);
            ptask->to = GTK_LABEL(gtk_label_new(task->dest_dir.c_str()));
            gtk_widget_set_halign(GTK_WIDGET(ptask->to), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(ptask->to), GTK_ALIGN_CENTER);
            gtk_label_set_ellipsize(ptask->to, PANGO_ELLIPSIZE_MIDDLE);
            gtk_label_set_selectable(ptask->to, true);
            gtk_grid_attach(grid, GTK_WIDGET(ptask->to), 1, row, 1, 1);
        }
        else
            ptask->to = nullptr;

        // Stats
        row++;
        label = GTK_LABEL(gtk_label_new("Progress:  "));
        gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_CENTER);
        gtk_grid_attach(grid, GTK_WIDGET(label), 0, row, 1, 1);
        ptask->current = GTK_LABEL(gtk_label_new(""));
        gtk_widget_set_halign(GTK_WIDGET(ptask->current), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(ptask->current), GTK_ALIGN_CENTER);
        gtk_label_set_ellipsize(ptask->current, PANGO_ELLIPSIZE_MIDDLE);
        gtk_label_set_selectable(ptask->current, true);
        gtk_grid_attach(grid, GTK_WIDGET(ptask->current), 1, row, 1, 1);
    }
    else
    {
        ptask->src_dir = nullptr;
        ptask->to = nullptr;
    }

    // Status
    row++;
    label = GTK_LABEL(gtk_label_new("Status:  "));
    gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_CENTER);
    gtk_grid_attach(grid, GTK_WIDGET(label), 0, row, 1, 1);
    const char* status;
    if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_PAUSE)
        status = "Paused";
    else if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
        status = "Queued";
    else
        status = "Running...";
    ptask->errors = GTK_LABEL(gtk_label_new(status));
    gtk_widget_set_halign(GTK_WIDGET(ptask->errors), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(ptask->errors), GTK_ALIGN_CENTER);
    gtk_label_set_ellipsize(ptask->errors, PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_selectable(ptask->errors, true);
    gtk_grid_attach(grid, GTK_WIDGET(ptask->errors), 1, row, 1, 1);

    /* Progress: */
    row++;
    ptask->progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ptask->progress_bar), true);
    gtk_progress_bar_set_pulse_step(ptask->progress_bar, 0.08);
    gtk_grid_attach(grid, GTK_WIDGET(ptask->progress_bar), 0, row, 1, 1);

    // Error log
    ptask->scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr));
    ptask->error_view = gtk_text_view_new_with_buffer(ptask->log_buf);
    // ubuntu shows input too small so use mininum height
    gtk_widget_set_size_request(GTK_WIDGET(ptask->error_view), -1, 70);
    gtk_widget_set_size_request(GTK_WIDGET(ptask->scroll), -1, 70);
    gtk_container_add(GTK_CONTAINER(ptask->scroll), ptask->error_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ptask->scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ptask->error_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ptask->error_view), false);

    g_signal_connect(ptask->error_view, "populate-popup", G_CALLBACK(on_view_popup), nullptr);
    GtkWidget* align = gtk_alignment_new(1, 1, 1, 1);
    gtk_alignment_set_padding(GTK_ALIGNMENT(align), 0, 0, 5, 5);
    gtk_container_add(GTK_CONTAINER(align), GTK_WIDGET(ptask->scroll));

    // Overwrite & Error
    GtkWidget* overwrite_align;
    if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        static constexpr std::array<std::string_view, 4> overwrite_options{
            "Ask",
            "Overwrite All",
            "Skip All",
            "Auto Rename",
        };
        static constexpr std::array<std::string_view, 3> error_options{
            "Stop If Error First",
            "Stop On Any Error",
            "Continue",
        };

        bool overtask = task->type == VFSFileTaskType::VFS_FILE_TASK_MOVE ||
                        task->type == VFSFileTaskType::VFS_FILE_TASK_COPY ||
                        task->type == VFSFileTaskType::VFS_FILE_TASK_LINK;
        ptask->overwrite_combo = gtk_combo_box_text_new();
        gtk_widget_set_focus_on_click(GTK_WIDGET(ptask->overwrite_combo), false);
        gtk_widget_set_sensitive(ptask->overwrite_combo, overtask);
        for (std::string_view overwrite_option: overwrite_options)
        {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ptask->overwrite_combo),
                                           overwrite_option.data());
        }
        if (overtask)
            gtk_combo_box_set_active(
                GTK_COMBO_BOX(ptask->overwrite_combo),
                task->overwrite_mode < overwrite_options.size() ? task->overwrite_mode : 0);
        g_signal_connect(G_OBJECT(ptask->overwrite_combo),
                         "changed",
                         G_CALLBACK(on_overwrite_combo_changed),
                         ptask);

        ptask->error_combo = gtk_combo_box_text_new();
        gtk_widget_set_focus_on_click(GTK_WIDGET(ptask->error_combo), false);
        for (std::string_view error_option: error_options)
        {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ptask->error_combo),
                                           error_option.data());
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(ptask->error_combo),
                                 ptask->err_mode < error_options.size() ? ptask->err_mode : 0);
        g_signal_connect(G_OBJECT(ptask->error_combo),
                         "changed",
                         G_CALLBACK(on_error_combo_changed),
                         ptask);
        GtkWidget* overwrite_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
        gtk_box_pack_start(GTK_BOX(overwrite_box),
                           GTK_WIDGET(ptask->overwrite_combo),
                           false,
                           true,
                           0);
        gtk_box_pack_start(GTK_BOX(overwrite_box), GTK_WIDGET(ptask->error_combo), false, true, 0);
        overwrite_align = gtk_alignment_new(1, 0, 1, 0);
        gtk_alignment_set_padding(GTK_ALIGNMENT(overwrite_align), 0, 0, 5, 5);
        gtk_container_add(GTK_CONTAINER(overwrite_align), GTK_WIDGET(overwrite_box));
    }
    else
    {
        overwrite_align = nullptr;
        ptask->overwrite_combo = nullptr;
        ptask->error_combo = nullptr;
    }

    // Pack
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(ptask->progress_dlg))),
                       GTK_WIDGET(grid),
                       false,
                       true,
                       0);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(ptask->progress_dlg))),
                       GTK_WIDGET(align),
                       true,
                       true,
                       0);
    if (overwrite_align)
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(ptask->progress_dlg))),
                           GTK_WIDGET(overwrite_align),
                           false,
                           true,
                           5);

    int win_width, win_height;
    if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        win_width = xset_get_int(XSetName::TASK_POP_TOP, XSetVar::S);
        win_height = xset_get_int(XSetName::TASK_POP_TOP, XSetVar::Z);
    }
    else
    {
        win_width = xset_get_int(XSetName::TASK_POP_TOP, XSetVar::X);
        win_height = xset_get_int(XSetName::TASK_POP_TOP, XSetVar::Y);
    }
    if (!win_width)
        win_width = 750;
    if (!win_height)
        win_height = -1;
    gtk_window_set_default_size(GTK_WINDOW(ptask->progress_dlg), win_width, win_height);
    gtk_button_box_set_layout(
        GTK_BUTTON_BOX(gtk_dialog_get_action_area(GTK_DIALOG(ptask->progress_dlg))),
        GTK_BUTTONBOX_END);
    if (xset_get_b(XSetName::TASK_POP_TOP))
        gtk_window_set_type_hint(GTK_WINDOW(ptask->progress_dlg), GDK_WINDOW_TYPE_HINT_DIALOG);
    else
        gtk_window_set_type_hint(GTK_WINDOW(ptask->progress_dlg), GDK_WINDOW_TYPE_HINT_NORMAL);
    if (xset_get_b(XSetName::TASK_POP_ABOVE))
        gtk_window_set_keep_above(GTK_WINDOW(ptask->progress_dlg), true);
    if (xset_get_b(XSetName::TASK_POP_STICK))
        gtk_window_stick(GTK_WINDOW(ptask->progress_dlg));
    gtk_window_set_gravity(GTK_WINDOW(ptask->progress_dlg), GDK_GRAVITY_NORTH_EAST);
    gtk_window_set_position(GTK_WINDOW(ptask->progress_dlg), GTK_WIN_POS_CENTER);
    gtk_window_set_role(GTK_WINDOW(ptask->progress_dlg), "task_dialog");

    //    gtk_dialog_set_default_response( ptask->progress_dlg, GTK_RESPONSE_OK );
    g_signal_connect(ptask->progress_dlg, "response", G_CALLBACK(on_progress_dlg_response), ptask);
    g_signal_connect(ptask->progress_dlg, "destroy", G_CALLBACK(on_progress_dlg_destroy), ptask);
    g_signal_connect(ptask->progress_dlg,
                     "delete-event",
                     G_CALLBACK(on_progress_dlg_delete_event),
                     ptask);
    // g_signal_connect( ptask->progress_dlg, "configure-event",
    //                  G_CALLBACK( on_progress_configure_event ), ptask );

    gtk_widget_show_all(ptask->progress_dlg);
    if (ptask->overwrite_combo && !xset_get_b(XSetName::TASK_POP_OVER))
        gtk_widget_hide(ptask->overwrite_combo);
    if (ptask->error_combo && !xset_get_b(XSetName::TASK_POP_ERR))
        gtk_widget_hide(ptask->error_combo);
    if (overwrite_align && !gtk_widget_get_visible(ptask->overwrite_combo) &&
        !gtk_widget_get_visible(ptask->error_combo))
        gtk_widget_hide(overwrite_align);
    gtk_widget_grab_focus(ptask->progress_btn_close);

    // icon
    set_progress_icon(ptask);

    // auto scroll - must be after show_all
    if (!task->exec_scroll_lock)
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(ptask->error_view),
                                     ptask->log_end,
                                     0.0,
                                     false,
                                     0,
                                     0);

    ptask->progress_count = 50; // trigger fast display
    // LOG_INFO("ptk_file_task_progress_open DONE");
}

static void
ptk_file_task_progress_update(PtkFileTask* ptask)
{
    if (!ptask->progress_dlg)
    {
        if (ptask->pause_change)
            ptask->pause_change = false; // stop elapsed timer
        return;
    }

    char* ufile_path;
    const char* window_title;
    std::string str;
    std::string str2;
    char percent_str[16];

    // LOG_INFO("ptk_file_task_progress_update ptask={:p}", ptask);

    VFSFileTask* task = ptask->task;

    // current file
    std::string usrc_dir;
    std::string udest;

    if (ptask->complete)
    {
        gtk_widget_set_sensitive(ptask->progress_btn_stop, false);
        gtk_widget_set_sensitive(ptask->progress_btn_pause, false);
        gtk_widget_set_sensitive(ptask->progress_btn_close, true);
        if (ptask->overwrite_combo)
            gtk_widget_set_sensitive(ptask->overwrite_combo, false);
        if (ptask->error_combo)
            gtk_widget_set_sensitive(ptask->error_combo, false);

        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
            ufile_path = nullptr;
        else
        {
            const std::string escaped_markup = Glib::Markup::escape_text(task->current_file);
            ufile_path = ztd::strdup(fmt::format("<b>{}</b>", escaped_markup));
        }

        if (ptask->aborted)
            window_title = "Stopped";
        else
        {
            if (task->err_count)
                window_title = "Errors";
            else
                window_title = "Done";
        }
        gtk_window_set_title(GTK_WINDOW(ptask->progress_dlg), window_title);
        if (!ufile_path)
        {
            const std::string escaped_markup = Glib::Markup::escape_text(window_title);
            ufile_path = ztd::strdup(fmt::format("<b>( {} )</b>", escaped_markup));
        }
    }
    else if (!task->current_file.empty())
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
        {
            // Copy: <src basename>
            const std::string name = Glib::filename_display_basename(task->current_file);
            const std::string escaped_markup = Glib::Markup::escape_text(name);
            ufile_path = ztd::strdup(fmt::format("<b>{}</b>", escaped_markup));

            // From: <src_dir>
            str = Glib::path_get_dirname(task->current_file);
            usrc_dir = Glib::filename_display_name(str);
            if (!ztd::same(usrc_dir, "/"))
            {
                usrc_dir = fmt::format("{}/", usrc_dir);
            }

            // To: <dest_dir> OR <dest_file>
            if (!task->current_dest.empty())
            {
                str = Glib::path_get_basename(task->current_file);
                str2 = Glib::path_get_basename(task->current_dest);
                if (!ztd::same(str, str2))
                {
                    // source and dest filenames differ, user renamed - show all
                    udest = Glib::filename_display_name(task->current_dest);
                }
                else
                {
                    // source and dest filenames same - show dest dir only
                    str = Glib::path_get_dirname(task->current_dest);
                    if (ztd::same(str, "/"))
                    {
                        udest = Glib::filename_display_name(str);
                    }
                    else
                    {
                        const std::string f = Glib::filename_display_name(str);
                        udest = fmt::format("{}/", f);
                    }
                }
            }
        }
        else
        {
            const std::string escaped_markup = Glib::Markup::escape_text(task->current_file);
            ufile_path = ztd::strdup(fmt::format("<b>{}</b>", escaped_markup));
        }
    }
    else
    {
        ufile_path = nullptr;
    }

    if (udest.empty() && !ptask->complete && !task->dest_dir.empty())
    {
        udest = Glib::filename_display_name(task->dest_dir);
        if (!ztd::same(udest, "/"))
        {
            const std::string f = Glib::filename_display_name(udest);
            udest = fmt::format("{}/", f);
        }
    }
    gtk_label_set_markup(ptask->from, ufile_path);
    if (ptask->src_dir)
        gtk_label_set_text(ptask->src_dir, usrc_dir.c_str());
    if (ptask->to)
        gtk_label_set_text(ptask->to, udest.c_str());
    free(ufile_path);

    // progress bar
    if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC || ptask->task->custom_percent)
    {
        if (task->percent >= 0)
        {
            if (task->percent > 100)
                task->percent = 100;
            gtk_progress_bar_set_fraction(ptask->progress_bar, ((double)task->percent) / 100);
            g_snprintf(percent_str, 16, "%d %%", task->percent);
            gtk_progress_bar_set_text(ptask->progress_bar, percent_str);
        }
        else
            gtk_progress_bar_set_fraction(ptask->progress_bar, 0);
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ptask->progress_bar), true);
    }
    else if (ptask->complete)
    {
        if (!ptask->task->custom_percent)
        {
            if (task->exec_is_error || ptask->aborted)
                gtk_progress_bar_set_fraction(ptask->progress_bar, 0);
            else
                gtk_progress_bar_set_fraction(ptask->progress_bar, 1);
            gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ptask->progress_bar), true);
        }
    }
    else if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC &&
             task->state_pause == VFSFileTaskState::VFS_FILE_TASK_RUNNING)
    {
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ptask->progress_bar), false);
        gtk_progress_bar_pulse(ptask->progress_bar);
    }

    // progress
    if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        std::string stats;

        if (ptask->complete)
        {
            if (ptask->pop_detail)
                stats = fmt::format("#{}  ({}) [{}] @avg {}",
                                    ptask->dsp_file_count,
                                    ptask->dsp_size_tally,
                                    ptask->dsp_elapsed,
                                    ptask->dsp_avgspeed);
            else
                stats = fmt::format("{} ({})", ptask->dsp_size_tally, ptask->dsp_avgspeed);
        }
        else
        {
            if (ptask->pop_detail)
                stats = fmt::format("#{} ({}) [{}] @cur {} ({}) @avg {} ({})",
                                    ptask->dsp_file_count,
                                    ptask->dsp_size_tally,
                                    ptask->dsp_elapsed,
                                    ptask->dsp_curspeed,
                                    ptask->dsp_curest,
                                    ptask->dsp_avgspeed,
                                    ptask->dsp_avgest);
            else
                stats = fmt::format("{}  ({})  {} remaining",
                                    ptask->dsp_size_tally,
                                    ptask->dsp_avgspeed,
                                    ptask->dsp_avgest);
        }
        gtk_label_set_text(ptask->current, stats.c_str());
    }

    // error/output log
    if (ptask->log_appended || ptask->force_scroll)
    {
        if (!task->exec_scroll_lock)
        {
            // scroll to end if scrollbar is mostly down or force_scroll
            GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(ptask->scroll);
            if (ptask->force_scroll ||
                gtk_adjustment_get_upper(adj) - gtk_adjustment_get_value(adj) <
                    gtk_adjustment_get_page_size(adj) + 40)
            {
                // LOG_INFO("    scroll to end line {}", ptask->log_end,
                // gtk_text_buffer_get_line_count(ptask->log_buf));
                gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(ptask->error_view),
                                             ptask->log_end,
                                             0.0,
                                             false,
                                             0,
                                             0);
            }
        }
        ptask->log_appended = false;
    }

    // icon
    if (ptask->pause_change || ptask->err_count != task->err_count)
    {
        ptask->pause_change = false;
        ptask->err_count = task->err_count;
        set_progress_icon(ptask);
    }

    // status
    std::string errs;
    if (ptask->complete)
    {
        if (ptask->aborted)
        {
            if (task->err_count && task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
            {
                if (ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_FIRST)
                    errs = "Error  ( Stop If First )";
                else if (ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_ANY)
                    errs = "Error  ( Stop On Any )";
                else
                    errs = fmt::format("Stopped with {} error", task->err_count);
            }
            else
                errs = "Stopped";
        }
        else
        {
            if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
            {
                if (task->err_count)
                    errs = fmt::format("Finished with {} error", task->err_count);
                else
                    errs = "Done";
            }
            else
            {
                if (task->exec_exit_status)
                    errs = fmt::format("Finished with error  ( exit status {} )",
                                       task->exec_exit_status);
                else if (task->exec_is_error)
                    errs = "Finished with error";
                else
                    errs = "Done";
            }
        }
    }
    else if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_PAUSE)
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
            errs = "Paused";
        else
        {
            if (task->exec_pid)
                errs = fmt::format("Paused  ( pid {} )", task->exec_pid);
            else
            {
                errs = fmt::format("Paused  ( exit status {} )", task->exec_exit_status);
                set_button_states(ptask);
            }
        }
    }
    else if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
            errs = ztd::strdup("Queued");
        else
        {
            if (task->exec_pid)
                errs = fmt::format("Queued  ( pid {} )", task->exec_pid);
            else
            {
                errs = fmt::format("Queued  ( exit status {} )", task->exec_exit_status);
                set_button_states(ptask);
            }
        }
    }
    else
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
        {
            if (task->err_count)
                errs = fmt::format("Running with {} error", task->err_count);
            else
                errs = "Running...";
        }
        else
        {
            if (task->exec_pid)
                errs = fmt::format("Running...  ( pid {} )", task->exec_pid);
            else
            {
                errs = fmt::format("Running...  ( exit status {} )", task->exec_exit_status);
                set_button_states(ptask);
            }
        }
    }
    gtk_label_set_text(ptask->errors, errs.c_str());
    // LOG_INFO("ptk_file_task_progress_update DONE ptask={:p}", fmt::ptr(ptask));
}

void
ptk_file_task_set_chmod(PtkFileTask* ptask, unsigned char* chmod_actions)
{
    vfs_file_task_set_chmod(ptask->task, chmod_actions);
}

void
ptk_file_task_set_chown(PtkFileTask* ptask, uid_t uid, gid_t gid)
{
    vfs_file_task_set_chown(ptask->task, uid, gid);
}

void
ptk_file_task_set_recursive(PtkFileTask* ptask, bool recursive)
{
    vfs_file_task_set_recursive(ptask->task, recursive);
}

static void
ptk_file_task_update(PtkFileTask* ptask)
{
    // LOG_INFO("ptk_file_task_update ptask={:p}", fmt::ptr(ptask));
    // calculate updated display data

    if (!ptk_file_task_trylock(ptask))
    {
        // LOG_INFO("UPDATE LOCKED");
        return;
    }

    VFSFileTask* task = ptask->task;
    off_t cur_speed;
    double timer_elapsed = task->timer.elapsed();

    if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        // test for zombie process
        int status = 0;
        if (!ptask->complete && task->exec_pid && waitpid(task->exec_pid, &status, WNOHANG))
        {
            // process is no longer running (defunct zombie)
            // glib should detect this but sometimes process goes defunct
            // with no watch callback, so remove it from task list
            if (task->child_watch)
            {
                g_source_remove(task->child_watch);
                task->child_watch = 0;
            }
            g_spawn_close_pid(task->exec_pid);
            if (task->exec_channel_out)
                g_io_channel_shutdown(task->exec_channel_out, true, nullptr);
            if (task->exec_channel_err)
                g_io_channel_shutdown(task->exec_channel_err, true, nullptr);
            task->exec_channel_out = task->exec_channel_err = nullptr;
            if (status)
            {
                if (WIFEXITED(status))
                    task->exec_exit_status = WEXITSTATUS(status);
                else
                    task->exec_exit_status = -1;
            }
            else
                task->exec_exit_status = 0;
            LOG_INFO("child ZOMBIED  pid={} exit_status={}",
                     task->exec_pid,
                     task->exec_exit_status);
            task->exec_pid = 0;
            ptask->complete = true;
        }
    }
    else
    {
        // cur speed
        if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_RUNNING)
        {
            double since_last = timer_elapsed - task->last_elapsed;
            if (since_last >= 2.0)
            {
                cur_speed = (task->progress - task->last_progress) / since_last;
                // LOG_INFO("( {} - {} ) / {} = {}", task->progress,
                //                task->last_progress, since_last, cur_speed);
                task->last_elapsed = timer_elapsed;
                task->last_speed = cur_speed;
                task->last_progress = task->progress;
            }
            else if (since_last > 0.1)
                cur_speed = (task->progress - task->last_progress) / since_last;
            else
                cur_speed = 0;
        }
        // calc percent
        int ipercent;
        if (task->total_size)
        {
            double dpercent = ((double)task->progress) / task->total_size;
            ipercent = (int)(dpercent * 100);
        }
        else
            ipercent = 50; // total_size calculation timed out
        if (ipercent != task->percent)
            task->percent = ipercent;
    }

    // elapsed
    unsigned int hours = timer_elapsed / 3600.0;
    std::string elapsed;
    std::string elapsed2;
    if (hours != 0)
        elapsed = fmt::format("{}", hours);
    unsigned int mins = (timer_elapsed - (hours * 3600)) / 60;
    if (hours > 0)
        elapsed2 = fmt::format("{}:{:02d}", elapsed, mins);
    else if (mins > 0)
        elapsed2 = fmt::format("{}", mins);
    else
        elapsed2 = elapsed;
    unsigned int secs = (timer_elapsed - (hours * 3600) - (mins * 60));
    const std::string elapsed3 = fmt::format("{}:{:02d}", elapsed2, secs);
    ptask->dsp_elapsed = elapsed3;

    if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
    {
        std::string file_count;
        std::string size_tally;
        std::string speed1;
        std::string speed2;
        std::string remain1;
        std::string remain2;

        std::string size_str;
        std::string size_str2;

        // count
        file_count = std::to_string(task->current_item);
        // size
        size_str = vfs_file_size_to_string_format(task->progress, true);
        if (task->total_size)
            size_str2 = vfs_file_size_to_string_format(task->total_size, true);
        else
            size_str2 = "??"; // total_size calculation timed out
        size_tally = fmt::format("{} / {}", size_str, size_str2);
        // cur speed display
        if (task->last_speed != 0)
            // use speed of last 2 sec interval if available
            cur_speed = task->last_speed;
        if (cur_speed == 0 || task->state_pause != VFSFileTaskState::VFS_FILE_TASK_RUNNING)
        {
            if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_PAUSE)
                speed1 = "paused";
            else if (task->state_pause == VFSFileTaskState::VFS_FILE_TASK_QUEUE)
                speed1 = "queued";
            else
                speed1 = "stalled";
        }
        else
        {
            size_str = vfs_file_size_to_string_format(cur_speed, true);
            speed1 = fmt::format("{}/s", size_str);
        }
        // avg speed
        std::time_t avg_speed;
        if (timer_elapsed > 0)
            avg_speed = task->progress / timer_elapsed;
        else
            avg_speed = 0;
        size_str2 = vfs_file_size_to_string_format(avg_speed, true);
        speed2 = fmt::format("{}/s", size_str2);

        // remain cur
        off_t remain;
        if (cur_speed > 0 && task->total_size != 0)
            remain = (task->total_size - task->progress) / cur_speed;
        else
            remain = 0;
        if (remain <= 0)
            remain1 = ""; // n/a
        else if (remain > 3599)
        {
            hours = remain / 3600;
            if (remain - (hours * 3600) > 1799)
                hours++;
            remain1 = fmt::format("{}/h", hours);
        }
        else if (remain > 59)
            remain1 =
                fmt::format("{}:{:02}", remain / 60, remain - ((unsigned int)(remain / 60) * 60));
        else
            remain1 = fmt::format(":{:02}", remain);

        // remain avg
        if (avg_speed > 0 && task->total_size != 0)
            remain = (task->total_size - task->progress) / avg_speed;
        else
            remain = 0;
        if (remain <= 0)
            remain2 = ""; // n/a
        else if (remain > 3599)
        {
            hours = remain / 3600;
            if (remain - (hours * 3600) > 1799)
                hours++;
            remain2 = fmt::format("{}/h", hours);
        }
        else if (remain > 59)
            remain2 =
                fmt::format("{}:{:02}", remain / 60, remain - ((unsigned int)(remain / 60) * 60));
        else
            remain2 = fmt::format(":{:02}", remain);

        ptask->dsp_file_count = file_count;
        ptask->dsp_size_tally = size_tally;
        ptask->dsp_curspeed = speed1;
        ptask->dsp_avgspeed = speed2;
        ptask->dsp_curest = remain1;
        ptask->dsp_avgest = remain2;
    }

    // move log lines from add_log_buf to log_buf
    if (gtk_text_buffer_get_char_count(task->add_log_buf))
    {
        GtkTextIter iter, siter;
        char* text;
        // get add_log text and delete
        gtk_text_buffer_get_start_iter(task->add_log_buf, &siter);
        gtk_text_buffer_get_iter_at_mark(task->add_log_buf, &iter, task->add_log_end);
        text = gtk_text_buffer_get_text(task->add_log_buf, &siter, &iter, false);
        gtk_text_buffer_delete(task->add_log_buf, &siter, &iter);
        // insert into log
        gtk_text_buffer_get_iter_at_mark(ptask->log_buf, &iter, ptask->log_end);
        gtk_text_buffer_insert(ptask->log_buf, &iter, text, -1);
        free(text);
        ptask->log_appended = true;

        // trim log ?  (less than 64K and 800 lines)
        if (gtk_text_buffer_get_char_count(ptask->log_buf) > 64000 ||
            gtk_text_buffer_get_line_count(ptask->log_buf) > 800)
        {
            if (gtk_text_buffer_get_char_count(ptask->log_buf) > 64000)
            {
                // trim to 50000 characters - handles single line flood
                gtk_text_buffer_get_iter_at_offset(ptask->log_buf,
                                                   &iter,
                                                   gtk_text_buffer_get_char_count(ptask->log_buf) -
                                                       50000);
            }
            else
                // trim to 700 lines
                gtk_text_buffer_get_iter_at_line(ptask->log_buf,
                                                 &iter,
                                                 gtk_text_buffer_get_line_count(ptask->log_buf) -
                                                     700);
            gtk_text_buffer_get_start_iter(ptask->log_buf, &siter);
            gtk_text_buffer_delete(ptask->log_buf, &siter, &iter);
            gtk_text_buffer_get_start_iter(ptask->log_buf, &siter);
            if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
                gtk_text_buffer_insert(
                    ptask->log_buf,
                    &siter,
                    "[ SNIP - additional output above has been trimmed from this log ]\n",
                    -1);
            else
                gtk_text_buffer_insert(
                    ptask->log_buf,
                    &siter,
                    "[ SNIP - additional errors above have been trimmed from this log ]\n",
                    -1);
        }

        if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC && task->exec_show_output)
        {
            if (!ptask->keep_dlg)
                ptask->keep_dlg = true;
            if (!ptask->progress_dlg)
            {
                // disable this line to open every time output occurs
                task->exec_show_output = false;
                ptk_file_task_progress_open(ptask);
            }
        }
    }

    if (!ptask->progress_dlg)
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC &&
            ptask->err_count != task->err_count)
        {
            ptask->keep_dlg = true;
            ptk_file_task_progress_open(ptask);
        }
        else if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC &&
                 ptask->err_count != task->err_count)
        {
            if (!ptask->aborted && task->exec_show_error)
            {
                ptask->keep_dlg = true;
                ptk_file_task_progress_open(ptask);
                // If error opens dialog after command finishes, gtk will not
                // scroll to end on initial attempts, so force_scroll
                // ensures it will try to scroll again - still sometimes
                // does not work
                ptask->force_scroll = ptask->complete && !task->exec_scroll_lock;
            }
        }
    }
    else
    {
        if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC &&
            ptask->err_count != task->err_count)
        {
            ptask->keep_dlg = true;
            if (ptask->complete || ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_ANY ||
                (task->error_first && ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_FIRST))
                gtk_window_present(GTK_WINDOW(ptask->progress_dlg));
        }
        else if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC &&
                 ptask->err_count != task->err_count)
        {
            if (!ptask->aborted && task->exec_show_error)
            {
                ptask->keep_dlg = true;
                gtk_window_present(GTK_WINDOW(ptask->progress_dlg));
            }
        }
    }

    ptk_file_task_progress_update(ptask);

    if (!ptask->timeout && !ptask->complete)
        main_task_view_update_task(ptask);

    vfs_file_task_unlock(task);
    // LOG_INFO("ptk_file_task_update DONE ptask={:p}", fmt::ptr(ptask));
}

static bool
on_vfs_file_task_state_cb(VFSFileTask* task, VFSFileTaskState state, void* state_data,
                          void* user_data)
{
    PtkFileTask* ptask = PTK_FILE_TASK(user_data);
    bool ret = true;

    switch (state)
    {
        case VFSFileTaskState::VFS_FILE_TASK_FINISH:
            // LOG_INFO("VFSFileTaskState::VFS_FILE_TASK_FINISH");

            ptask->complete = true;

            vfs_file_task_lock(task);
            if (task->type != VFSFileTaskType::VFS_FILE_TASK_EXEC)
                task->current_file.clear();
            ptask->progress_count = 50; // trigger fast display
            vfs_file_task_unlock(task);
            // gtk_signal_emit_by_name( G_OBJECT( ptask->signal_widget ), "task-notify",
            //                                                                 ptask );
            break;
        case VFSFileTaskState::VFS_FILE_TASK_QUERY_OVERWRITE:
            // 0; GThread *self = g_thread_self ();
            // LOG_INFO("TASK_THREAD = {:p}", fmt::ptr(self));
            vfs_file_task_lock(task);
            ptask->query_new_dest = (char**)state_data;
            *ptask->query_new_dest = nullptr;
            ptask->query_cond = g_cond_new();
            task->timer.stop();
            g_cond_wait(ptask->query_cond, task->mutex);
            g_cond_free(ptask->query_cond);
            ptask->query_cond = nullptr;
            ret = ptask->query_ret;
            task->last_elapsed = task->timer.elapsed();
            task->last_progress = task->progress;
            task->last_speed = 0;
            task->timer.start();
            vfs_file_task_unlock(task);
            break;
        case VFSFileTaskState::VFS_FILE_TASK_ERROR:
            // LOG_INFO("VFSFileTaskState::VFS_FILE_TASK_ERROR");
            vfs_file_task_lock(task);
            task->err_count++;
            // LOG_INFO("    ptask->item_count = {}", task->current_item );

            if (task->type == VFSFileTaskType::VFS_FILE_TASK_EXEC)
            {
                task->exec_is_error = true;
                ret = false;
            }
            else if (ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_ANY ||
                     (task->error_first &&
                      ptask->err_mode == PTKFileTaskPtaskError::PTASK_ERROR_FIRST))
            {
                ret = false;
                ptask->aborted = true;
            }
            ptask->progress_count = 50; // trigger fast display

            vfs_file_task_unlock(task);

            if (xset_get_b(XSetName::TASK_Q_PAUSE))
            {
                // pause all queued
                main_task_pause_all_queued(ptask);
            }
            break;
        case VFSFileTaskState::VFS_FILE_TASK_RUNNING:
        case VFSFileTaskState::VFS_FILE_TASK_SIZE_TIMEOUT:
        case VFSFileTaskState::VFS_FILE_TASK_PAUSE:
        case VFSFileTaskState::VFS_FILE_TASK_QUEUE:
        default:
            break;
    }

    return ret; /* return true to continue */
}

enum PTKFileTaskResponse
{
    RESPONSE_OVERWRITE = 1 << 0,
    RESPONSE_OVERWRITEALL = 1 << 1,
    RESPONSE_RENAME = 1 << 2,
    RESPONSE_SKIP = 1 << 3,
    RESPONSE_SKIPALL = 1 << 4,
    RESPONSE_AUTO_RENAME = 1 << 5,
    RESPONSE_AUTO_RENAME_ALL = 1 << 6,
    RESPONSE_PAUSE = 1 << 7
};

static bool
on_query_input_keypress(GtkWidget* widget, GdkEventKey* event, PtkFileTask* ptask)
{
    (void)ptask;
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
    {
        // User pressed enter in rename/overwrite dialog
        char* new_name = multi_input_get_text(widget);
        const char* old_name = (const char*)g_object_get_data(G_OBJECT(widget), "old_name");
        GtkWidget* dlg = gtk_widget_get_toplevel(widget);
        if (!GTK_IS_DIALOG(dlg))
            return true;
        if (new_name && new_name[0] != '\0' && strcmp(new_name, old_name))
            gtk_dialog_response(GTK_DIALOG(dlg), RESPONSE_RENAME);
        else
            gtk_dialog_response(GTK_DIALOG(dlg), RESPONSE_AUTO_RENAME);
        free(new_name);
        return true;
    }
    return false;
}

static void
on_multi_input_changed(GtkWidget* input_buf, GtkWidget* query_input)
{
    (void)input_buf;
    char* new_name = multi_input_get_text(query_input);
    const char* old_name = (const char*)g_object_get_data(G_OBJECT(query_input), "old_name");
    bool can_rename = new_name && (0 != strcmp(new_name, old_name));
    free(new_name);
    GtkWidget* dlg = gtk_widget_get_toplevel(query_input);
    if (!GTK_IS_DIALOG(dlg))
        return;
    GtkWidget* rename_button = GTK_WIDGET(g_object_get_data(G_OBJECT(dlg), "rename_button"));
    if (GTK_IS_WIDGET(rename_button))
        gtk_widget_set_sensitive(rename_button, can_rename);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(dlg), RESPONSE_OVERWRITE, !can_rename);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(dlg), RESPONSE_OVERWRITEALL, !can_rename);
}

static void
query_overwrite_response(GtkDialog* dlg, int response, PtkFileTask* ptask)
{
    std::string file_name;
    char* str;

    switch (response)
    {
        case RESPONSE_OVERWRITEALL:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_OVERWRITE_ALL);
            if (ptask->progress_dlg)
                gtk_combo_box_set_active(GTK_COMBO_BOX(ptask->overwrite_combo),
                                         VFSFileTaskOverwriteMode::VFS_FILE_TASK_OVERWRITE_ALL);
            break;
        case RESPONSE_OVERWRITE:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_OVERWRITE);
            break;
        case RESPONSE_SKIPALL:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_SKIP_ALL);
            if (ptask->progress_dlg)
                gtk_combo_box_set_active(GTK_COMBO_BOX(ptask->overwrite_combo),
                                         VFSFileTaskOverwriteMode::VFS_FILE_TASK_SKIP_ALL);
            break;
        case RESPONSE_SKIP:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_SKIP);
            break;
        case RESPONSE_AUTO_RENAME_ALL:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_AUTO_RENAME);
            if (ptask->progress_dlg)
                gtk_combo_box_set_active(GTK_COMBO_BOX(ptask->overwrite_combo),
                                         VFSFileTaskOverwriteMode::VFS_FILE_TASK_AUTO_RENAME);
            break;
        case RESPONSE_AUTO_RENAME:
        case RESPONSE_RENAME:
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_RENAME);
            if (response == RESPONSE_AUTO_RENAME)
            {
                GtkWidget* auto_button =
                    GTK_WIDGET(g_object_get_data(G_OBJECT(dlg), "auto_button"));
                str = gtk_widget_get_tooltip_text(auto_button);
            }
            else
            {
                GtkWidget* query_input =
                    GTK_WIDGET(g_object_get_data(G_OBJECT(dlg), "query_input"));
                str = multi_input_get_text(query_input);
            }
            file_name = Glib::filename_from_utf8(str);
            if (str && !file_name.empty() && !ptask->task->current_dest.empty())
            {
                const std::string dir_name = Glib::path_get_dirname(ptask->task->current_dest);
                const std::string path = Glib::build_filename(dir_name, file_name);
                *ptask->query_new_dest = ztd::strdup(path);
            }
            free(str);
            break;
        case RESPONSE_PAUSE:
            ptk_file_task_pause(ptask, VFSFileTaskState::VFS_FILE_TASK_PAUSE);
            main_task_start_queued(ptask->task_view, ptask);
            vfs_file_task_set_overwrite_mode(ptask->task,
                                             VFSFileTaskOverwriteMode::VFS_FILE_TASK_RENAME);
            ptask->restart_timeout = false;
            break;
        case GTK_RESPONSE_DELETE_EVENT: // escape was pressed or window closed
        case GTK_RESPONSE_CANCEL:
            ptask->task->abort = true;
            break;
        default:
            break;
    }

    // save size
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(dlg), &allocation);
    if (allocation.width && allocation.height)
    {
        int has_overwrite_btn =
            GPOINTER_TO_INT((void*)g_object_get_data(G_OBJECT(dlg), "has_overwrite_btn"));
        xset_set(XSetName::TASK_POPUPS,
                 has_overwrite_btn ? XSetVar::X : XSetVar::S,
                 std::to_string(allocation.width));
        xset_set(XSetName::TASK_POPUPS,
                 has_overwrite_btn ? XSetVar::Y : XSetVar::Z,
                 std::to_string(allocation.height));
    }

    gtk_widget_destroy(GTK_WIDGET(dlg));

    if (ptask->query_cond)
    {
        ptk_file_task_lock(ptask);
        ptask->query_ret =
            (response != GTK_RESPONSE_DELETE_EVENT) && (response != GTK_RESPONSE_CANCEL);
        // g_cond_broadcast( ptask->query_cond );
        g_cond_signal(ptask->query_cond);
        ptk_file_task_unlock(ptask);
    }
    if (ptask->restart_timeout)
    {
        ptask->timeout = g_timeout_add(500, (GSourceFunc)ptk_file_task_add_main, (void*)ptask);
    }
    ptask->progress_count = 50;
    ptask->progress_timer = g_timeout_add(50, (GSourceFunc)on_progress_timer, ptask);
}

static void
on_query_button_press(GtkWidget* widget, PtkFileTask* ptask)
{
    GtkWidget* dlg = gtk_widget_get_toplevel(widget);
    if (!GTK_IS_DIALOG(dlg))
        return;
    GtkWidget* rename_button = GTK_WIDGET(g_object_get_data(G_OBJECT(dlg), "rename_button"));
    GtkWidget* auto_button = GTK_WIDGET(g_object_get_data(G_OBJECT(dlg), "auto_button"));
    if (!rename_button || !auto_button)
        return;
    int response;
    if (widget == rename_button)
        response = RESPONSE_RENAME;
    else if (widget == auto_button)
        response = RESPONSE_AUTO_RENAME;
    else
        response = RESPONSE_AUTO_RENAME_ALL;
    query_overwrite_response(GTK_DIALOG(dlg), response, ptask);
}

static void
query_overwrite(PtkFileTask* ptask)
{
    // TODO convert to gtk_builder (glade file)

    // LOG_INFO("query_overwrite ptask={:p}", fmt::ptr(ptask));
    GtkWidget* dlg;
    GtkWidget* parent_win;
    GtkTextIter iter;

    bool has_overwrite_btn = true;
    bool different_files;
    bool is_src_dir;
    bool is_dest_dir;

    struct stat src_stat;
    struct stat dest_stat;

    std::string title;
    std::string message;
    std::string from_size_str;
    std::string to_size_str;
    std::string from_disp;

    if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_MOVE)
        from_disp = "Moving from directory:";
    else if (ptask->task->type == VFSFileTaskType::VFS_FILE_TASK_LINK)
        from_disp = "Linking from directory:";
    else
        from_disp = "Copying from directory:";

    different_files = (!ztd::same(ptask->task->current_file, ptask->task->current_dest));

    lstat(ptask->task->current_file.c_str(), &src_stat);
    lstat(ptask->task->current_dest.c_str(), &dest_stat);

    is_src_dir = !!S_ISDIR(dest_stat.st_mode);
    is_dest_dir = !!S_ISDIR(src_stat.st_mode);

    if (different_files && is_dest_dir == is_src_dir)
    {
        if (is_dest_dir)
        {
            /* Ask the user whether to overwrite dir content or not */
            title = "Directory Exists";
            message = "<b>Directory already exists.</b>  Please rename or select an action.";
        }
        else
        {
            /* Ask the user whether to overwrite the file or not */
            char buf[64];
            std::string dest_size;
            std::string dest_time;
            std::string src_size;
            std::string src_time;
            std::string src_rel;
            std::string src_rel_size;
            std::string src_rel_time;
            std::string src_link;
            std::string dest_link;
            std::string link_warn;

            std::string size_str;

            if (S_ISLNK(src_stat.st_mode))
                src_link = "\t<b>( link )</b>";
            if (S_ISLNK(dest_stat.st_mode))
                dest_link = "\t<b>( link )</b>";
            if (S_ISLNK(src_stat.st_mode) && !S_ISLNK(dest_stat.st_mode))
                link_warn = "\t<b>! overwrite file with link !</b>";
            if (src_stat.st_size == dest_stat.st_size)
                src_size = "<b>( same size )</b>";
            else
            {
                size_str = vfs_file_size_to_string_format(src_stat.st_size, true);
                src_size = fmt::format("{}\t( {} bytes )", size_str, src_stat.st_size);
                if (src_stat.st_size > dest_stat.st_size)
                    src_rel_size = "larger";
                else
                    src_rel_size = "smaller";
            }
            if (src_stat.st_mtime == dest_stat.st_mtime)
                src_time = "<b>( same time )</b>\t";
            else
            {
                strftime(buf,
                         sizeof(buf),
                         app_settings.get_date_format().c_str(),
                         std::localtime(&src_stat.st_mtime));
                src_time = buf;
                if (src_stat.st_mtime > dest_stat.st_mtime)
                    src_rel_time = "newer";
                else
                    src_rel_time = "older";
            }
            size_str = vfs_file_size_to_string_format(dest_stat.st_size, true);
            dest_size = fmt::format("{}\t( {} bytes )", size_str, dest_stat.st_size);
            strftime(buf,
                     sizeof(buf),
                     app_settings.get_date_format().c_str(),
                     std::localtime(&dest_stat.st_mtime));
            dest_time = buf;

            src_rel = fmt::format("{}{}{}{}{}",
                                  !src_rel_time.empty() || !src_rel_size.empty() ? "<b>( " : "",
                                  !src_rel_time.empty() ? src_rel_time : "",
                                  !src_rel_time.empty() && !src_rel_size.empty() ? " &amp; " : "",
                                  !src_rel_size.empty() ? src_rel_size : "",
                                  !src_rel_time.empty() || !src_rel_size.empty() ? " )</b> " : "");

            from_size_str = fmt::format("\t{}\t{}{}{}{}",
                                        src_time,
                                        src_size,
                                        !src_rel.empty() ? "\t" : "",
                                        src_rel,
                                        src_link);
            to_size_str = fmt::format("\t{}\t{}{}",
                                      dest_time,
                                      dest_size,
                                      !dest_link.empty() ? dest_link : link_warn);

            title = "Filename Exists";
            message = "<b>Filename already exists.</b>  Please rename or select an action.";
        }
    }
    else
    { /* Rename is required */
        has_overwrite_btn = false;
        title = "Rename Required";
        if (!different_files)
            from_disp = "In directory:";
        message = "<b>Filename already exists.</b>  Please rename or select an action.";
    }

    // filenames
    char* base_name = ztd::strdup(Glib::path_get_basename(ptask->task->current_dest));
    char* base_name_disp = ztd::strdup(Glib::filename_display_name(base_name)); // auto free
    char* src_dir = ztd::strdup(Glib::path_get_dirname(ptask->task->current_file));
    char* src_dir_disp = ztd::strdup(Glib::filename_display_name(src_dir));
    char* dest_dir = ztd::strdup(Glib::path_get_dirname(ptask->task->current_dest));
    char* dest_dir_disp = ztd::strdup(Glib::filename_display_name(dest_dir));

    const auto namepack = get_name_extension(base_name);
    const std::string name = namepack.first;
    const std::string ext = namepack.second;

    char* ext_disp = !ext.empty() ? ztd::strdup(Glib::filename_display_name(ext)) : nullptr;
    char* unique_name = vfs_file_task_get_unique_name(dest_dir, name.c_str(), ext.c_str());
    char* new_name_plain =
        unique_name ? ztd::strdup(Glib::path_get_basename(unique_name)) : nullptr;
    char* new_name =
        new_name_plain ? ztd::strdup(Glib::filename_display_name(new_name_plain)) : nullptr;

    int pos = ext_disp ? g_utf8_strlen(base_name_disp, -1) - g_utf8_strlen(ext_disp, -1) - 1 : -1;

    free(base_name);
    free(unique_name);
    free(ext_disp);
    free(src_dir);
    free(dest_dir);
    free(new_name_plain);

    // create dialog
    if (ptask->progress_dlg)
        parent_win = GTK_WIDGET(ptask->progress_dlg);
    else
        parent_win = GTK_WIDGET(ptask->parent_window);
    dlg = gtk_dialog_new_with_buttons(
        title.c_str(),
        GTK_WINDOW(parent_win),
        GtkDialogFlags(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        nullptr,
        nullptr);

    g_signal_connect(G_OBJECT(dlg), "response", G_CALLBACK(query_overwrite_response), ptask);
    gtk_window_set_resizable(GTK_WINDOW(dlg), true);
    gtk_window_set_title(GTK_WINDOW(dlg), title.c_str());
    gtk_window_set_type_hint(GTK_WINDOW(dlg), GDK_WINDOW_TYPE_HINT_DIALOG);
    gtk_window_set_gravity(GTK_WINDOW(dlg), GDK_GRAVITY_NORTH_EAST);
    gtk_window_set_position(GTK_WINDOW(dlg), GTK_WIN_POS_CENTER);
    gtk_window_set_role(GTK_WINDOW(dlg), "overwrite_dialog");

    int width, height;
    if (has_overwrite_btn)
    {
        width = xset_get_int(XSetName::TASK_POPUPS, XSetVar::X);
        height = xset_get_int(XSetName::TASK_POPUPS, XSetVar::Y);
    }
    else
    {
        width = xset_get_int(XSetName::TASK_POPUPS, XSetVar::S);
        height = xset_get_int(XSetName::TASK_POPUPS, XSetVar::Z);
    }
    if (width && height)
        gtk_window_set_default_size(GTK_WINDOW(dlg), width, height);
    else if (!has_overwrite_btn)
        gtk_widget_set_size_request(GTK_WIDGET(dlg), 600, -1);

    GtkWidget* align = gtk_alignment_new(1, 0, 1, 1);
    gtk_alignment_set_padding(GTK_ALIGNMENT(align), 0, 14, 7, 7);
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(align), vbox);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dlg))), align, true, true, 0);

    // buttons
    if (has_overwrite_btn)
    {
        gtk_dialog_add_buttons(GTK_DIALOG(dlg),
                               "_Overwrite",
                               RESPONSE_OVERWRITE,
                               "Overwrite _All",
                               RESPONSE_OVERWRITEALL,
                               nullptr);
    }

    GtkWidget* btn_pause = gtk_dialog_add_button(GTK_DIALOG(dlg), "_Pause", RESPONSE_PAUSE);
    gtk_dialog_add_buttons(GTK_DIALOG(dlg),
                           "_Skip",
                           RESPONSE_SKIP,
                           "S_kip All",
                           RESPONSE_SKIPALL,
                           "Cancel",
                           GTK_RESPONSE_CANCEL,
                           nullptr);

    // xset_t set = xset_get(XSetName::TASK_PAUSE);
    gtk_widget_set_sensitive(btn_pause, !!ptask->task_view);

    // labels
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(nullptr), false, true, 0);
    GtkWidget* msg = gtk_label_new(nullptr);
    gtk_label_set_markup(GTK_LABEL(msg), message.c_str());
    gtk_widget_set_halign(GTK_WIDGET(msg), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(msg), GTK_ALIGN_START);
    gtk_widget_set_can_focus(msg, false);
    gtk_box_pack_start(GTK_BOX(vbox), msg, false, true, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(nullptr), false, true, 0);
    GtkWidget* from_label = gtk_label_new(nullptr);
    gtk_label_set_markup(GTK_LABEL(from_label), from_disp.c_str());
    gtk_widget_set_halign(GTK_WIDGET(from_label), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(from_label), GTK_ALIGN_START);
    gtk_widget_set_can_focus(from_label, false);
    gtk_box_pack_start(GTK_BOX(vbox), from_label, false, true, 0);

    GtkWidget* from_dir = gtk_label_new(src_dir_disp);
    gtk_widget_set_halign(GTK_WIDGET(from_dir), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(from_dir), GTK_ALIGN_START);
    gtk_label_set_ellipsize(GTK_LABEL(from_dir), PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_selectable(GTK_LABEL(from_dir), true);
    gtk_box_pack_start(GTK_BOX(vbox), from_dir, false, true, 0);

    if (!from_size_str.empty())
    {
        GtkWidget* from_size = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(from_size), from_size_str.c_str());
        gtk_widget_set_halign(GTK_WIDGET(from_size), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(from_size), GTK_ALIGN_END);
        gtk_label_set_selectable(GTK_LABEL(from_size), true);
        gtk_box_pack_start(GTK_BOX(vbox), from_size, false, true, 0);
    }

    if (has_overwrite_btn || different_files)
    {
        gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(nullptr), false, true, 0);
        GtkWidget* to_label = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(to_label), "To directory:");
        gtk_widget_set_halign(GTK_WIDGET(to_label), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(to_label), GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(vbox), to_label, false, true, 0);

        GtkWidget* to_dir = gtk_label_new(dest_dir_disp);
        gtk_widget_set_halign(GTK_WIDGET(to_dir), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(to_dir), GTK_ALIGN_START);
        gtk_label_set_ellipsize(GTK_LABEL(to_dir), PANGO_ELLIPSIZE_MIDDLE);
        gtk_label_set_selectable(GTK_LABEL(to_dir), true);
        gtk_box_pack_start(GTK_BOX(vbox), to_dir, false, true, 0);

        if (!to_size_str.empty())
        {
            GtkWidget* to_size = gtk_label_new(nullptr);
            gtk_label_set_markup(GTK_LABEL(to_size), to_size_str.c_str());
            gtk_widget_set_halign(GTK_WIDGET(to_size), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(to_size), GTK_ALIGN_END);
            gtk_label_set_selectable(GTK_LABEL(to_size), true);
            gtk_box_pack_start(GTK_BOX(vbox), to_size, false, true, 0);
        }
    }

    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(nullptr), false, true, 0);
    GtkWidget* name_label = gtk_label_new(nullptr);
    gtk_label_set_markup(GTK_LABEL(name_label),
                         is_dest_dir ? "<b>Directory Name:</b>" : "<b>Filename:</b>");
    gtk_widget_set_halign(GTK_WIDGET(name_label), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(name_label), GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), name_label, false, true, 0);

    // name input
    GtkWidget* scroll = gtk_scrolled_window_new(nullptr, nullptr);
    GtkWidget* query_input =
        GTK_WIDGET(multi_input_new(GTK_SCROLLED_WINDOW(scroll), base_name_disp));
    g_signal_connect(G_OBJECT(query_input),
                     "key-press-event",
                     G_CALLBACK(on_query_input_keypress),
                     ptask);
    GtkWidget* input_buf = GTK_WIDGET(gtk_text_view_get_buffer(GTK_TEXT_VIEW(query_input)));
    gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(input_buf), &iter, pos);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(input_buf), &iter);
    g_signal_connect(G_OBJECT(input_buf),
                     "changed",
                     G_CALLBACK(on_multi_input_changed),
                     query_input);
    g_object_set_data_full(G_OBJECT(query_input), "old_name", base_name_disp, free);
    gtk_widget_set_size_request(GTK_WIDGET(query_input), -1, 60);
    gtk_widget_set_size_request(GTK_WIDGET(scroll), -1, 60);
    GtkTextBuffer* buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(query_input));
    GtkTextMark* mark = gtk_text_buffer_get_insert(buf);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(query_input), mark, 0, true, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(scroll), true, true, 4);

    // extra buttons
    GtkWidget* rename_button = gtk_button_new_with_mnemonic(" _Rename ");
    gtk_widget_set_sensitive(rename_button, false);
    g_signal_connect(G_OBJECT(rename_button), "clicked", G_CALLBACK(on_query_button_press), ptask);
    GtkWidget* auto_button = gtk_button_new_with_mnemonic(" A_uto Rename ");
    g_signal_connect(G_OBJECT(auto_button), "clicked", G_CALLBACK(on_query_button_press), ptask);
    gtk_widget_set_tooltip_text(auto_button, new_name);
    GtkWidget* auto_all_button = gtk_button_new_with_mnemonic(" Auto Re_name All ");
    g_signal_connect(G_OBJECT(auto_all_button),
                     "clicked",
                     G_CALLBACK(on_query_button_press),
                     ptask);
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(rename_button), false, true, 0);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(auto_button), false, true, 0);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(auto_all_button), false, true, 0);
    align = gtk_alignment_new(1, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), GTK_WIDGET(hbox));
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(align), false, true, 0);

    free(src_dir_disp);
    free(dest_dir_disp);
    free(new_name);

    // update displays (mutex is already locked)
    ptask->dsp_curspeed = "stalled";
    ptk_file_task_progress_update(ptask);
    if (ptask->task_view &&
        gtk_widget_get_visible(gtk_widget_get_parent(GTK_WIDGET(ptask->task_view))))
        main_task_view_update_task(ptask);

    // show dialog
    g_object_set_data(G_OBJECT(dlg), "rename_button", rename_button);
    g_object_set_data(G_OBJECT(dlg), "auto_button", auto_button);
    g_object_set_data(G_OBJECT(dlg), "query_input", query_input);
    g_object_set_data(G_OBJECT(dlg), "has_overwrite_btn", GINT_TO_POINTER(has_overwrite_btn));
    gtk_widget_show_all(GTK_WIDGET(dlg));

    gtk_widget_grab_focus(query_input);

    // cannot run gtk_dialog_run here because it does not unlock a low level
    // mutex when run from inside the timer handler
    return;
}
