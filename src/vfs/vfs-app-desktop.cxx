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

#include <string>
#include <string_view>

#include <span>

#include <array>
#include <vector>

#include <map>

#include <optional>

#include <fmt/format.h>

#include <glibmm.h>
#include <glibmm/keyfile.h>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

// sfm breaks vfs independence for exec_in_terminal
#include "ptk/ptk-file-task.hxx"

#include "vfs/vfs-utils.hxx"
#include "vfs/vfs-user-dirs.hxx"
#include "vfs/vfs-app-desktop.hxx"

std::map<std::string, vfs::desktop> desktops_map;

vfs::desktop
vfs_get_desktop(std::string_view desktop_file)
{
    if (desktops_map.count(desktop_file.data()) == 1)
    {
        // ztd::logger::info("cached vfs_get_desktop={}", desktop_file);
        return desktops_map.at(desktop_file.data());
    }
    // ztd::logger::info("new vfs_get_desktop={}", desktop_file);

    vfs::desktop desktop = std::make_shared<VFSAppDesktop>(desktop_file);

    desktops_map.insert({desktop_file.data(), desktop});

    return desktop;
}

VFSAppDesktop::VFSAppDesktop(std::string_view open_file_name) noexcept
{
    // ztd::logger::info("VFSAppDesktop constructor = {}", open_file_name);

    bool load;
    const auto kf = Glib::KeyFile::create();

    if (Glib::path_is_absolute(open_file_name.data()))
    {
        this->file_name = Glib::path_get_basename(open_file_name.data());
        this->full_path = open_file_name.data();
        load = kf->load_from_file(open_file_name.data(), Glib::KeyFile::Flags::NONE);
    }
    else
    {
        this->file_name = open_file_name.data();
        const std::string relative_path = Glib::build_filename("applications", this->file_name);
        load = kf->load_from_data_dirs(relative_path, this->full_path, Glib::KeyFile::Flags::NONE);
    }

    if (!load)
    {
        ztd::logger::warn("Failed to load desktop file {}", open_file_name);
        this->exec = this->file_name;
        return;
    }

    static const std::string desktop_entry = "Desktop Entry";

    // Keys not loaded from .desktop files
    // - Hidden
    // - OnlyShowIn
    // - NotShowIn
    // - DBusActivatable
    // - URL
    // - PrefersNonDefaultGPU
    // - SingleMainWindow

    try
    {
        const Glib::ustring g_type = kf->get_string(desktop_entry, "Type");
        if (!g_type.empty())
        {
            this->type = g_type;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_name = kf->get_string(desktop_entry, "Name");
        if (!g_name.empty())
        {
            this->name = g_name;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_generic_name = kf->get_string(desktop_entry, "GenericName");
        if (!g_generic_name.empty())
        {
            this->generic_name = g_generic_name;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        this->no_display = kf->get_boolean(desktop_entry, "NoDisplay");
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_comment = kf->get_string(desktop_entry, "Comment");
        if (!g_comment.empty())
        {
            this->comment = g_comment;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_icon = kf->get_string(desktop_entry, "Icon");
        if (!g_icon.empty())
        {
            this->icon = g_icon;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_try_exec = kf->get_string(desktop_entry, "TryExec");
        if (!g_try_exec.empty())
        {
            this->try_exec = g_try_exec;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_exec = kf->get_string(desktop_entry, "Exec");
        if (!g_exec.empty())
        {
            this->exec = g_exec;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_path = kf->get_string(desktop_entry, "Path");
        if (!g_path.empty())
        {
            this->path = g_path;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        this->terminal = kf->get_boolean(desktop_entry, "Terminal");
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_actions = kf->get_string(desktop_entry, "Actions");
        if (!g_actions.empty())
        {
            this->actions = g_actions;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_mime_type = kf->get_string(desktop_entry, "MimeType");
        if (!g_mime_type.empty())
        {
            this->mime_type = g_mime_type; // TODO vector
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_categories = kf->get_string(desktop_entry, "Categories");
        if (!g_categories.empty())
        {
            this->categories = g_categories;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_keywords = kf->get_string(desktop_entry, "Keywords");
        if (!g_keywords.empty())
        {
            this->keywords = g_keywords;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        this->startup_notify = kf->get_boolean(desktop_entry, "StartupNotify");
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }

    try
    {
        const Glib::ustring g_startup_wm_class = kf->get_string(desktop_entry, "StartupWMClass");
        if (!g_startup_wm_class.empty())
        {
            this->startup_wm_class = g_startup_wm_class;
        }
    }
    catch (const Glib::KeyFileError& e)
    { /* Desktop Missing Key, Use Default init */
    }
}

const std::string&
VFSAppDesktop::get_name() const noexcept
{
    return this->file_name;
}

const std::string&
VFSAppDesktop::get_disp_name() const noexcept
{
    if (!this->name.empty())
    {
        return this->name;
    }
    return this->file_name;
}

const std::string&
VFSAppDesktop::get_exec() const noexcept
{
    return this->exec;
}

bool
VFSAppDesktop::use_terminal() const noexcept
{
    return this->terminal;
}

const std::string&
VFSAppDesktop::get_full_path() const noexcept
{
    return this->full_path;
}

const std::string&
VFSAppDesktop::get_icon_name() const noexcept
{
    return this->icon;
}

GdkPixbuf*
VFSAppDesktop::get_icon(i32 size) const noexcept
{
    GdkPixbuf* desktop_icon = nullptr;

    if (!this->icon.empty())
    {
        desktop_icon = vfs_load_icon(this->icon, size);
    }

    // fallback to generic icon
    if (!desktop_icon)
    {
        desktop_icon = vfs_load_icon("application-x-executable", size);
        // fallback to generic icon
        if (!desktop_icon)
        {
            desktop_icon = vfs_load_icon("gnome-mime-application-x-executable", size);
        }
    }
    return desktop_icon;
}

bool
VFSAppDesktop::open_multiple_files() const noexcept
{
    if (this->exec.empty())
    {
        return false;
    }

    static constexpr std::array<std::string_view, 2> keys{"%U", "%F"};
    if (ztd::contains(this->exec, keys))
    {
        return true;
    }

    return false;
}

const std::optional<std::vector<std::vector<std::string>>>
VFSAppDesktop::app_exec_generate_desktop_argv(const std::span<const std::string> file_list,
                                              bool quote_file_list) const noexcept
{
    // https://standards.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html#exec-variables

    std::vector<std::vector<std::string>> commands = {{ztd::split(this->exec, " ")}};

    bool add_files = false;

    static constexpr std::array<std::string_view, 2> open_files_keys{"%F", "%U"};
    if (ztd::contains(this->exec, open_files_keys))
    {
        // %F and %U must always be at the end
        if (!ztd::endswith(this->exec, open_files_keys))
        {
            ztd::logger::error("Malformed desktop file, %F and %U must always be at the end : {}",
                               this->full_path);
            return std::nullopt;
        }

        for (auto& argv : commands)
        {
            argv.pop_back(); // remove open_files_key
            for (const std::string_view file : file_list)
            {
                if (quote_file_list)
                {
                    argv.emplace_back(ztd::shell::quote(file));
                }
                else
                {
                    argv.emplace_back(file.data());
                }
            }
        }

        add_files = true;
    }

    static constexpr std::array<std::string_view, 2> open_file_keys{"%f", "%u"};
    if (ztd::contains(this->exec, open_file_keys))
    {
        // %f and %u must always be at the end
        if (!ztd::endswith(this->exec, open_file_keys))
        {
            ztd::logger::error("Malformed desktop file, %f and %u must always be at the end : {}",
                               this->full_path);
            return std::nullopt;
        }

        // desktop files with these keys can only open one file.
        // spawn multiple copies of the program for each selected file
        commands.insert(commands.begin(), file_list.size() - 1, commands.front());

        for (auto& argv : commands)
        {
            argv.pop_back(); // remove open_file_key
            for (const std::string_view file : file_list)
            {
                if (quote_file_list)
                {
                    argv.emplace_back(ztd::shell::quote(file));
                }
                else
                {
                    argv.emplace_back(file.data());
                }
            }
        }
        add_files = true;
    }

    if (!add_files && !file_list.empty())
    {
        ztd::logger::warn(
            "Trying to open a desktop file with a file list without file/url keys : {}",
            this->full_path);
    }

    if (ztd::contains(this->exec, "%c"))
    {
        for (auto& argv : commands)
        {
            for (const auto [index, arg] : ztd::enumerate(argv))
            {
                if (!ztd::contains(arg, "%c"))
                {
                    argv[index] = this->get_disp_name();
                    break;
                }
            }
        }
    }

    if (ztd::contains(this->exec, "%k"))
    {
        for (auto& argv : commands)
        {
            for (const auto [index, arg] : ztd::enumerate(argv))
            {
                if (ztd::contains(arg, "%k"))
                {
                    argv[index] = this->get_full_path();
                    break;
                }
            }
        }
    }

    if (ztd::contains(this->exec, "%i"))
    {
        for (auto& argv : commands)
        {
            for (const auto [index, arg] : ztd::enumerate(argv))
            {
                if (ztd::contains(arg, "%i"))
                {
                    argv[index] = fmt::format("--icon {}", this->get_icon_name());
                    break;
                }
            }
        }
    }

    return commands;
}

void
VFSAppDesktop::exec_in_terminal(std::string_view cwd, std::string_view command) const noexcept
{
    // task
    PtkFileTask* ptask = ptk_file_exec_new(this->get_disp_name(), cwd, nullptr, nullptr);

    ptask->task->exec_command = command;

    ptask->task->exec_terminal = true;
    // ptask->task->exec_keep_terminal = true;  // for test only
    ptask->task->exec_sync = false;
    ptask->task->exec_export = false;

    ptk_file_task_run(ptask);
}

bool
VFSAppDesktop::open_files(std::string_view working_dir,
                          const std::span<const std::string> file_paths) const
{
    if (this->exec.empty())
    {
        const std::string msg = fmt::format("Command not found\n\n{}", this->file_name);
        throw VFSAppDesktopException(msg);
    }

    if (this->open_multiple_files())
    {
        this->exec_desktop(working_dir, file_paths);
    }
    else
    {
        // app does not accept multiple files, so run multiple times
        for (const std::string_view open_file : file_paths)
        {
            const std::vector<std::string> open_files{open_file.data()};
            this->exec_desktop(working_dir, open_files);
        }
    }
    return true;
}

void
VFSAppDesktop::exec_desktop(std::string_view working_dir,
                            const std::span<const std::string> file_paths) const noexcept
{
    const auto desktop_commands =
        this->app_exec_generate_desktop_argv(file_paths, this->use_terminal());
    if (!desktop_commands)
    {
        return;
    }

    if (this->use_terminal())
    {
        for (const auto& argv : desktop_commands.value())
        {
            const std::string command = ztd::join(argv, " ");
            this->exec_in_terminal(!this->path.empty() ? this->path : working_dir, command);
        }
    }
    else
    {
        for (const auto& argv : desktop_commands.value())
        {
            Glib::spawn_async_with_pipes(!this->path.empty() ? this->path : working_dir.data(),
                                         argv,
                                         Glib::SpawnFlags::SEARCH_PATH |
                                             Glib::SpawnFlags::STDOUT_TO_DEV_NULL |
                                             Glib::SpawnFlags::STDERR_TO_DEV_NULL,
                                         Glib::SlotSpawnChildSetup(),
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         nullptr);
        }
    }
}
