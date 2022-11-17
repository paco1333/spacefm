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

#include <fmt/format.h>

#include <toml.hpp> // toml11

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "xset/xset.hxx"

#include "ptk/ptk-error.hxx"

#include "vfs/vfs-user-dir.hxx"

#include "settings/app.hxx"
#include "settings/config-load.hxx"
#include "settings/disk-format.hxx"

#ifdef HAVE_DEPRECATED_INI_LOADING
static void xset_parse(std::string& line);

using SettingsParseFunc = void (*)(std::string& line);

static void
parse_general_settings(std::string& line)
{
    usize sep = line.find("=");
    if (sep == std::string::npos)
        return;

    line = ztd::strip(line);

    if (line.at(0) == '#')
        return;

    std::string token = line.substr(0, sep);
    std::string value = line.substr(sep + 1, std::string::npos - 1);

    // remove any quotes
    value = ztd::replace(value, "\"", "");

    if (value.empty())
        return;

    if (ztd::same(token, INI_KEY_SHOW_THUMBNAIL))
        app_settings.set_show_thumbnail(std::stoi(value));
    else if (ztd::same(token, INI_KEY_MAX_THUMB_SIZE))
        app_settings.set_max_thumb_size(std::stoi(value) << 10);
    else if (ztd::same(token, INI_KEY_ICON_SIZE_BIG))
        app_settings.set_icon_size_big(std::stoi(value));
    else if (ztd::same(token, INI_KEY_ICON_SIZE_SMALL))
        app_settings.set_icon_size_small(std::stoi(value));
    else if (ztd::same(token, INI_KEY_ICON_SIZE_TOOL))
        app_settings.set_icon_size_tool(std::stoi(value));
    else if (ztd::same(token, INI_KEY_SINGLE_CLICK))
        app_settings.set_single_click(std::stoi(value));
    else if (ztd::same(token, INI_KEY_NO_SINGLE_HOVER))
        app_settings.set_single_hover(std::stoi(value));
    else if (ztd::same(token, INI_KEY_SORT_ORDER))
        app_settings.set_sort_order(std::stoi(value));
    else if (ztd::same(token, INI_KEY_SORT_TYPE))
        app_settings.set_sort_type(std::stoi(value));
    else if (ztd::same(token, INI_KEY_USE_SI_PREFIX))
        app_settings.set_use_si_prefix(std::stoi(value));
    else if (ztd::same(token, INI_KEY_NO_EXECUTE))
        app_settings.set_click_executes(!std::stoi(value));
    else if (ztd::same(token, INI_KEY_NO_CONFIRM))
        app_settings.set_confirm(!std::stoi(value));
    else if (ztd::same(token, INI_KEY_NO_CONFIRM_TRASH))
        app_settings.set_confirm_trash(!std::stoi(value));
}

static void
parse_window_state(std::string& line)
{
    usize sep = line.find("=");
    if (sep == std::string::npos)
        return;

    line = ztd::strip(line);

    if (line.at(0) == '#')
        return;

    std::string token = line.substr(0, sep);
    std::string value = line.substr(sep + 1, std::string::npos - 1);

    // remove any quotes
    value = ztd::replace(value, "\"", "");

    if (value.empty())
        return;

    if (ztd::same(token, INI_KEY_WIDTH))
        app_settings.set_width(std::stoi(value));
    else if (ztd::same(token, INI_KEY_HEIGHT))
        app_settings.set_height(std::stoi(value));
    else if (ztd::same(token, INI_KEY_MAXIMIZED))
        app_settings.set_maximized(std::stoi(value));
}

static void
parse_interface_settings(std::string& line)
{
    usize sep = line.find("=");
    if (sep == std::string::npos)
        return;

    line = ztd::strip(line);

    if (line.at(0) == '#')
        return;

    std::string token = line.substr(0, sep);
    std::string value = line.substr(sep + 1, std::string::npos - 1);

    // remove any quotes
    value = ztd::replace(value, "\"", "");

    if (value.empty())
        return;

    if (ztd::same(token, INI_KEY_SHOW_TABS))
        app_settings.set_always_show_tabs(std::stoi(value));
    else if (ztd::same(token, INI_KEY_SHOW_CLOSE))
        app_settings.set_show_close_tab_buttons(std::stoi(value));
}

static void
xset_parse(std::string& line)
{
    usize sep = line.find("=");
    if (sep == std::string::npos)
        return;

    usize sep2 = line.find("-");
    if (sep2 == std::string::npos)
        return;

    line = ztd::strip(line);

    if (line.at(0) == '#')
        return;

    std::string token = line.substr(0, sep2);
    std::string value = line.substr(sep + 1, std::string::npos - 1);
    std::string token_var = line.substr(sep2 + 1, sep - sep2 - 1);

    XSetVar var;
    try
    {
        var = xset_get_xsetvar_from_name(token_var);
    }
    catch (const std::logic_error& e)
    {
        std::string msg = fmt::format("XSet parse error:\n\n{}", e.what());
        ptk_show_error(nullptr, "Error", e.what());
        return;
    }

    // remove any quotes
    value = ztd::replace(value, "\"", "");

    if (value.empty())
        return;

    xset_t set = xset_get(token);
    if (ztd::startswith(set->name, "cstm_") || ztd::startswith(set->name, "hand_"))
    { // custom
        if (set->lock)
            set->lock = false;
    }
    else
    { // normal (lock)
        xset_set_var(set, var, value);
    }
}
#endif // Deprecated INI loader - end

static u64
get_config_file_version(const toml::value& tbl)
{
    if (!tbl.contains(TOML_SECTION_VERSION))
    {
        ztd::logger::error("config missing TOML section [{}]", TOML_SECTION_VERSION);
        return 0;
    }

    const auto& version = toml::find(tbl, TOML_SECTION_VERSION);

    const auto config_version = toml::find<u64>(version, TOML_KEY_VERSION);
    return config_version;
}

static void
config_parse_general(const toml::value& tbl, u64 version)
{
    (void)version;

    if (!tbl.contains(TOML_SECTION_GENERAL))
    {
        ztd::logger::error("config missing TOML section [{}]", TOML_SECTION_GENERAL);
        return;
    }

    const auto& section = toml::find(tbl, TOML_SECTION_GENERAL);

    if (section.contains(TOML_KEY_SHOW_THUMBNAIL))
    {
        const auto show_thumbnail = toml::find<bool>(section, TOML_KEY_SHOW_THUMBNAIL);
        app_settings.set_show_thumbnail(show_thumbnail);
    }

    if (section.contains(TOML_KEY_MAX_THUMB_SIZE))
    {
        const auto max_thumb_size = toml::find<u64>(section, TOML_KEY_MAX_THUMB_SIZE);
        app_settings.set_max_thumb_size(max_thumb_size << 10);
    }

    if (section.contains(TOML_KEY_ICON_SIZE_BIG))
    {
        const auto icon_size_big = toml::find<u64>(section, TOML_KEY_ICON_SIZE_BIG);
        app_settings.set_icon_size_big(icon_size_big);
    }

    if (section.contains(TOML_KEY_ICON_SIZE_SMALL))
    {
        const auto icon_size_small = toml::find<u64>(section, TOML_KEY_ICON_SIZE_SMALL);
        app_settings.set_icon_size_small(icon_size_small);
    }

    if (section.contains(TOML_KEY_ICON_SIZE_TOOL))
    {
        const auto icon_size_tool = toml::find<u64>(section, TOML_KEY_ICON_SIZE_TOOL);
        app_settings.set_icon_size_tool(icon_size_tool);
    }

    if (section.contains(TOML_KEY_SINGLE_CLICK))
    {
        const auto single_click = toml::find<bool>(section, TOML_KEY_SINGLE_CLICK);
        app_settings.set_single_click(single_click);
    }

    if (section.contains(TOML_KEY_SINGLE_HOVER))
    {
        const auto single_hover = toml::find<bool>(section, TOML_KEY_SINGLE_HOVER);
        app_settings.set_single_hover(single_hover);
    }

    if (section.contains(TOML_KEY_SORT_ORDER))
    {
        const auto sort_order = toml::find<u64>(section, TOML_KEY_SORT_ORDER);
        app_settings.set_sort_order(sort_order);
    }

    if (section.contains(TOML_KEY_SORT_TYPE))
    {
        const auto sort_type = toml::find<u64>(section, TOML_KEY_SORT_TYPE);
        app_settings.set_sort_type(sort_type);
    }

    if (section.contains(TOML_KEY_USE_SI_PREFIX))
    {
        const auto use_si_prefix = toml::find<bool>(section, TOML_KEY_USE_SI_PREFIX);
        app_settings.set_use_si_prefix(use_si_prefix);
    }

    if (section.contains(TOML_KEY_CLICK_EXECUTE))
    {
        const auto click_executes = toml::find<bool>(section, TOML_KEY_CLICK_EXECUTE);
        app_settings.set_click_executes(click_executes);
    }

    if (section.contains(TOML_KEY_CONFIRM))
    {
        const auto confirm = toml::find<bool>(section, TOML_KEY_CONFIRM);
        app_settings.set_confirm(confirm);
    }

    if (section.contains(TOML_KEY_CONFIRM_DELETE))
    {
        const auto confirm_delete = toml::find<bool>(section, TOML_KEY_CONFIRM_DELETE);
        app_settings.set_confirm_delete(confirm_delete);
    }

    if (section.contains(TOML_KEY_CONFIRM_TRASH))
    {
        const auto confirm_trash = toml::find<bool>(section, TOML_KEY_CONFIRM_TRASH);
        app_settings.set_confirm_trash(confirm_trash);
    }
}

static void
config_parse_window(const toml::value& tbl, u64 version)
{
    (void)version;

    if (!tbl.contains(TOML_SECTION_WINDOW))
    {
        ztd::logger::error("config missing TOML section [{}]", TOML_SECTION_WINDOW);
        return;
    }

    const auto& section = toml::find(tbl, TOML_SECTION_WINDOW);

    if (section.contains(TOML_KEY_HEIGHT))
    {
        const auto height = toml::find<u64>(section, TOML_KEY_HEIGHT);
        app_settings.set_height(height);
    }

    if (section.contains(TOML_KEY_WIDTH))
    {
        const auto width = toml::find<u64>(section, TOML_KEY_WIDTH);
        app_settings.set_width(width);
    }

    if (section.contains(TOML_KEY_MAXIMIZED))
    {
        const auto maximized = toml::find<bool>(section, TOML_KEY_MAXIMIZED);
        app_settings.set_maximized(maximized);
    }
}

static void
config_parse_interface(const toml::value& tbl, u64 version)
{
    (void)version;

    if (!tbl.contains(TOML_SECTION_INTERFACE))
    {
        ztd::logger::error("config missing TOML section [{}]", TOML_SECTION_INTERFACE);
        return;
    }

    const auto& section = toml::find(tbl, TOML_SECTION_INTERFACE);

    if (section.contains(TOML_KEY_SHOW_TABS))
    {
        const auto always_show_tabs = toml::find<bool>(section, TOML_KEY_SHOW_TABS);
        app_settings.set_always_show_tabs(always_show_tabs);
    }

    if (section.contains(TOML_KEY_SHOW_CLOSE))
    {
        const auto show_close_tab_buttons = toml::find<bool>(section, TOML_KEY_SHOW_CLOSE);
        app_settings.set_show_close_tab_buttons(show_close_tab_buttons);
    }
}

static void
config_parse_xset(const toml::value& tbl, u64 version)
{
    (void)version;

    // loop over all of [[XSet]]
    for (const auto& section : toml::find<toml::array>(tbl, TOML_SECTION_XSET))
    {
        // get [XSet.name] and all vars
        for (const auto& [toml_name, toml_vars] : section.as_table())
        {
            const std::string name = toml_name.data();
            const xset_t set = xset_get(name);

            // get var and value
            for (const auto& [toml_var, toml_value] : toml_vars.as_table())
            {
                const std::string setvar = toml_var.data();
                const std::string value =
                    ztd::strip(toml::format(toml_value, std::numeric_limits<usize>::max()), "\"");

                // LOG_INFO("name: {} | var: {} | value: {}", name, setvar, value);

                XSetVar var;
                try
                {
                    var = xset_get_xsetvar_from_name(setvar);
                }
                catch (const std::logic_error& e)
                {
                    const std::string msg = fmt::format("XSet parse error:\n\n{}", e.what());
                    ptk_show_error(nullptr, "Error", e.what());
                    return;
                }

                if (ztd::startswith(set->name, "cstm_") || ztd::startswith(set->name, "hand_"))
                { // custom
                    if (set->lock)
                        set->lock = false;
                    xset_set_var(set, var, value);
                }
                else
                { // normal (lock)
                    xset_set_var(set, var, value);
                }
            }
        }
    }
}

#if defined(HAVE_DEPRECATED_INI_LOADING)

void
load_user_confing(std::string_view session, bool load_deprecated_ini_config)
{
    if (!load_deprecated_ini_config)
    { // TOML
        // LOG_INFO("Parse TOML");

        try
        {
            const auto tbl = toml::parse(session.data());
            // DEBUG
            // std::cout << "###### TOML PARSE ######" << "\n\n";
            // std::cout << tbl << "\n\n";

            const u64 version = get_config_file_version(tbl);

            config_parse_general(tbl, version);
            config_parse_window(tbl, version);
            config_parse_interface(tbl, version);
            config_parse_xset(tbl, version);
        }
        catch (const toml::syntax_error& e)
        {
            LOG_ERROR("Config file parsing failed: {}", e.what());
            return;
        }
    }
    else
    { // INI
        // LOG_INFO("Parse INI");

        std::string line;
        std::ifstream file(session.data());
        if (file.is_open())
        {
            SettingsParseFunc func = nullptr;

            while (std::getline(file, line))
            {
                if (line.empty())
                    continue;

                if (line.at(0) == '[')
                {
                    if (ztd::same(line, INI_SECTION_GENERAL))
                        func = &parse_general_settings;
                    else if (ztd::same(line, INI_SECTION_WINDOW))
                        func = &parse_window_state;
                    else if (ztd::same(line, INI_SECTION_INTERFACE))
                        func = &parse_interface_settings;
                    else if (ztd::same(line, INI_SECTION_MOD))
                        func = &xset_parse;
                    else
                        func = nullptr;
                    continue;
                }

                if (func)
                    (*func)(line);
            }
        }
        file.close();
    }
}

#else

void
load_user_confing(std::string_view session)
{
    try
    {
        const auto tbl = toml::parse(session.data());
        // DEBUG
        // std::cout << "###### TOML PARSE ######" << "\n\n";
        // std::cout << tbl << "\n\n";

        const u64 version = get_config_file_version(tbl);

        config_parse_general(tbl, version);
        config_parse_window(tbl, version);
        config_parse_interface(tbl, version);
        config_parse_xset(tbl, version);
    }
    catch (const toml::syntax_error& e)
    {
        ztd::logger::error("Config file parsing failed: {}", e.what());
        return;
    }
}

#endif