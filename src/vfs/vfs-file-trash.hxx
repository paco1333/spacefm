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

#include <string>
#include <string_view>

#include <map>

#include <memory>
#include <optional>

// trash directories. There might be several on a system:
//
// One in $XDG_DATA_HOME/VFSTrash or ~/.local/share/VFSTrash
// if $XDG_DATA_HOME is not set
//
// Every mountpoint will get a trash directory at $TOPLEVEL/.VFSTrash-$UID.
class VFSTrashDir
{
  public:
    // Create the trash directory and subdirectories if they do not exist.
    VFSTrashDir(std::string_view path) noexcept;
    ~VFSTrashDir() = default;

    // Get a unique name for use within the trash directory
    const std::string unique_name(std::string_view path) const noexcept;

    void create_trash_dir() const noexcept;

    // Create a .trashinfo file for a file or directory 'path'
    void create_trash_info(std::string_view path, std::string_view target_name) const noexcept;

    // Move a file or directory into the trash directory
    void move(std::string_view path, std::string_view target_name) const noexcept;

  protected:
    // Create a directory if it does not exist
    static void check_dir_exists(std::string_view dir) noexcept;

    // Data Members

    // the device (as returned from stat()) for this trash directory
    dev_t trash_device;

    // the full path for this trash directory
    std::string trash_path{};
    // the path of the "files" subdirectory of this trash dir
    std::string files_path{};
    // the path of the "info" subdirectory of this trash dir
    std::string info_path{};
};

// This class implements some of the XDG VFSTrash specification:
//
// https://standards.freedesktop.org/trash-spec/trashspec-1.0.html
class VFSTrash
{
  public:
    // Move a file or directory into the trash.
    static bool trash(std::string_view path) noexcept;

    // Restore a file or directory from the trash to its original location.
    // Currently a NOOP
    static bool restore(std::string_view path) noexcept;

    // Empty all trash cans
    // Currently a NOOP
    static void empty() noexcept;

  private:
    // Not for public use. Use instance() or the static methods instead.
    VFSTrash() noexcept;
    virtual ~VFSTrash() = default;

    // Return the singleton object for this class. The first use will create
    // the singleton. Notice that the static methods all access the singleton,
    // too, so the first call to any of those static methods will already
    // create the singleton.
    // static std::shared_ptr<VFSTrash> instance() noexcept;
    static VFSTrash* instance() noexcept;
    static VFSTrash* single_instance;

    // return the device of the file or directory
    static std::optional<dev_t> device(std::string_view path) noexcept;

    // Find the toplevel directory (mount point) for the device that 'path' is on.
    static const std::string toplevel(std::string_view path) noexcept;

    // Return the trash dir to use for 'path'.
    std::shared_ptr<VFSTrashDir> trash_dir(std::string_view path) noexcept;

    // Data Members
    std::map<dev_t, std::shared_ptr<VFSTrashDir>> trash_dirs;

}; // class VFSTrash
