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

#include <string_view>

#include <array>

#include <unordered_map>

#include <cassert>

#include <magic_enum.hpp>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "types.hxx"

#include "xset/xset-lookup.hxx"

struct panel_lookup_data
{
    std::array<xset::name, MAX_PANELS> panel{};
    std::array<std::array<xset::name, 4>, MAX_PANELS> panel_mode{};
};

const std::unordered_map<xset::panel, panel_lookup_data> xset_panel_lookup{
    {
        xset::panel::show,
        {
            {
                xset::name::panel1_show,
                xset::name::panel2_show,
                xset::name::panel3_show,
                xset::name::panel4_show,
            },
        },
        // No panel mode for this one.
    },
    {
        xset::panel::show_toolbox,
        {
            {
                xset::name::panel1_show_toolbox,
                xset::name::panel2_show_toolbox,
                xset::name::panel3_show_toolbox,
                xset::name::panel4_show_toolbox,
            },
            {{
                {
                    xset::name::panel1_show_toolbox0,
                    xset::name::panel1_show_toolbox1,
                    xset::name::panel1_show_toolbox2,
                    xset::name::panel1_show_toolbox3,
                },
                {
                    xset::name::panel2_show_toolbox0,
                    xset::name::panel2_show_toolbox1,
                    xset::name::panel2_show_toolbox2,
                    xset::name::panel2_show_toolbox3,
                },
                {
                    xset::name::panel3_show_toolbox0,
                    xset::name::panel3_show_toolbox1,
                    xset::name::panel3_show_toolbox2,
                    xset::name::panel3_show_toolbox3,
                },
                {
                    xset::name::panel4_show_toolbox0,
                    xset::name::panel4_show_toolbox1,
                    xset::name::panel4_show_toolbox2,
                    xset::name::panel4_show_toolbox3,
                },
            }},
        },
    },
    {
        xset::panel::show_devmon,
        {
            {
                xset::name::panel1_show_devmon,
                xset::name::panel2_show_devmon,
                xset::name::panel3_show_devmon,
                xset::name::panel4_show_devmon,
            },
            {{
                {
                    xset::name::panel1_show_devmon0,
                    xset::name::panel1_show_devmon1,
                    xset::name::panel1_show_devmon2,
                    xset::name::panel1_show_devmon3,
                },
                {
                    xset::name::panel2_show_devmon0,
                    xset::name::panel2_show_devmon1,
                    xset::name::panel2_show_devmon2,
                    xset::name::panel2_show_devmon3,
                },
                {
                    xset::name::panel3_show_devmon0,
                    xset::name::panel3_show_devmon1,
                    xset::name::panel3_show_devmon2,
                    xset::name::panel3_show_devmon3,
                },
                {
                    xset::name::panel4_show_devmon0,
                    xset::name::panel4_show_devmon1,
                    xset::name::panel4_show_devmon2,
                    xset::name::panel4_show_devmon3,
                },
            }},
        },
    },
    {
        xset::panel::show_dirtree,
        {
            {
                xset::name::panel1_show_dirtree,
                xset::name::panel2_show_dirtree,
                xset::name::panel3_show_dirtree,
                xset::name::panel4_show_dirtree,
            },
            {{
                {
                    xset::name::panel1_show_dirtree0,
                    xset::name::panel1_show_dirtree1,
                    xset::name::panel1_show_dirtree2,
                    xset::name::panel1_show_dirtree3,
                },
                {
                    xset::name::panel2_show_dirtree0,
                    xset::name::panel2_show_dirtree1,
                    xset::name::panel2_show_dirtree2,
                    xset::name::panel2_show_dirtree3,
                },
                {
                    xset::name::panel3_show_dirtree0,
                    xset::name::panel3_show_dirtree1,
                    xset::name::panel3_show_dirtree2,
                    xset::name::panel3_show_dirtree3,
                },
                {
                    xset::name::panel4_show_dirtree0,
                    xset::name::panel4_show_dirtree1,
                    xset::name::panel4_show_dirtree2,
                    xset::name::panel4_show_dirtree3,
                },
            }},
        },
    },
    {
        xset::panel::slider_positions,
        {
            {
                xset::name::panel1_slider_positions,
                xset::name::panel2_slider_positions,
                xset::name::panel3_slider_positions,
                xset::name::panel4_slider_positions,
            },
            {{
                {
                    xset::name::panel1_slider_positions0,
                    xset::name::panel1_slider_positions1,
                    xset::name::panel1_slider_positions2,
                    xset::name::panel1_slider_positions3,
                },
                {
                    xset::name::panel2_slider_positions0,
                    xset::name::panel2_slider_positions1,
                    xset::name::panel2_slider_positions2,
                    xset::name::panel2_slider_positions3,
                },
                {
                    xset::name::panel3_slider_positions0,
                    xset::name::panel3_slider_positions1,
                    xset::name::panel3_slider_positions2,
                    xset::name::panel3_slider_positions3,
                },
                {
                    xset::name::panel4_slider_positions0,
                    xset::name::panel4_slider_positions1,
                    xset::name::panel4_slider_positions2,
                    xset::name::panel4_slider_positions3,
                },
            }},
        },
    },
    {
        xset::panel::list_detailed,
        {
            {
                xset::name::panel1_list_detailed,
                xset::name::panel2_list_detailed,
                xset::name::panel3_list_detailed,
                xset::name::panel4_list_detailed,
            },
            {{
                {
                    xset::name::panel1_list_detailed0,
                    xset::name::panel1_list_detailed1,
                    xset::name::panel1_list_detailed2,
                    xset::name::panel1_list_detailed3,
                },
                {
                    xset::name::panel2_list_detailed0,
                    xset::name::panel2_list_detailed1,
                    xset::name::panel2_list_detailed2,
                    xset::name::panel2_list_detailed3,
                },
                {
                    xset::name::panel3_list_detailed0,
                    xset::name::panel3_list_detailed1,
                    xset::name::panel3_list_detailed2,
                    xset::name::panel3_list_detailed3,
                },
                {
                    xset::name::panel4_list_detailed0,
                    xset::name::panel4_list_detailed1,
                    xset::name::panel4_list_detailed2,
                    xset::name::panel4_list_detailed3,
                },
            }},
        },
    },
    {
        xset::panel::list_icons,
        {
            {
                xset::name::panel1_list_icons,
                xset::name::panel2_list_icons,
                xset::name::panel3_list_icons,
                xset::name::panel4_list_icons,
            },
            {{
                {
                    xset::name::panel1_list_icons0,
                    xset::name::panel1_list_icons1,
                    xset::name::panel1_list_icons2,
                    xset::name::panel1_list_icons3,
                },
                {
                    xset::name::panel2_list_icons0,
                    xset::name::panel2_list_icons1,
                    xset::name::panel2_list_icons2,
                    xset::name::panel2_list_icons3,
                },
                {
                    xset::name::panel3_list_icons0,
                    xset::name::panel3_list_icons1,
                    xset::name::panel3_list_icons2,
                    xset::name::panel3_list_icons3,
                },
                {
                    xset::name::panel4_list_icons0,
                    xset::name::panel4_list_icons1,
                    xset::name::panel4_list_icons2,
                    xset::name::panel4_list_icons3,
                },
            }},
        },
    },
    {
        xset::panel::list_compact,
        {
            {
                xset::name::panel1_list_compact,
                xset::name::panel2_list_compact,
                xset::name::panel3_list_compact,
                xset::name::panel4_list_compact,
            },
            {{
                {
                    xset::name::panel1_list_compact0,
                    xset::name::panel1_list_compact1,
                    xset::name::panel1_list_compact2,
                    xset::name::panel1_list_compact3,
                },
                {
                    xset::name::panel2_list_compact0,
                    xset::name::panel2_list_compact1,
                    xset::name::panel2_list_compact2,
                    xset::name::panel2_list_compact3,
                },
                {
                    xset::name::panel3_list_compact0,
                    xset::name::panel3_list_compact1,
                    xset::name::panel3_list_compact2,
                    xset::name::panel3_list_compact3,
                },
                {
                    xset::name::panel4_list_compact0,
                    xset::name::panel4_list_compact1,
                    xset::name::panel4_list_compact2,
                    xset::name::panel4_list_compact3,
                },
            }},
        },
    },
    {
        xset::panel::list_large,
        {
            {
                xset::name::panel1_list_large,
                xset::name::panel2_list_large,
                xset::name::panel3_list_large,
                xset::name::panel4_list_large,
            },
            {{
                {
                    xset::name::panel1_list_large0,
                    xset::name::panel1_list_large1,
                    xset::name::panel1_list_large2,
                    xset::name::panel1_list_large3,
                },
                {
                    xset::name::panel2_list_large0,
                    xset::name::panel2_list_large1,
                    xset::name::panel2_list_large2,
                    xset::name::panel2_list_large3,
                },
                {
                    xset::name::panel3_list_large0,
                    xset::name::panel3_list_large1,
                    xset::name::panel3_list_large2,
                    xset::name::panel3_list_large3,
                },
                {
                    xset::name::panel4_list_large0,
                    xset::name::panel4_list_large1,
                    xset::name::panel4_list_large2,
                    xset::name::panel4_list_large3,
                },
            }},
        },
    },
    {
        xset::panel::show_hidden,
        {
            {
                xset::name::panel1_show_hidden,
                xset::name::panel2_show_hidden,
                xset::name::panel3_show_hidden,
                xset::name::panel4_show_hidden,
            },
            {{
                {
                    xset::name::panel1_show_hidden0,
                    xset::name::panel1_show_hidden1,
                    xset::name::panel1_show_hidden2,
                    xset::name::panel1_show_hidden3,
                },
                {
                    xset::name::panel2_show_hidden0,
                    xset::name::panel2_show_hidden1,
                    xset::name::panel2_show_hidden2,
                    xset::name::panel2_show_hidden3,
                },
                {
                    xset::name::panel3_show_hidden0,
                    xset::name::panel3_show_hidden1,
                    xset::name::panel3_show_hidden2,
                    xset::name::panel3_show_hidden3,
                },
                {
                    xset::name::panel4_show_hidden0,
                    xset::name::panel4_show_hidden1,
                    xset::name::panel4_show_hidden2,
                    xset::name::panel4_show_hidden3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_name,
        {
            {
                xset::name::panel1_detcol_name,
                xset::name::panel2_detcol_name,
                xset::name::panel3_detcol_name,
                xset::name::panel4_detcol_name,
            },
            {{
                {
                    xset::name::panel1_detcol_name0,
                    xset::name::panel1_detcol_name1,
                    xset::name::panel1_detcol_name2,
                    xset::name::panel1_detcol_name3,
                },
                {
                    xset::name::panel2_detcol_name0,
                    xset::name::panel2_detcol_name1,
                    xset::name::panel2_detcol_name2,
                    xset::name::panel2_detcol_name3,
                },
                {
                    xset::name::panel3_detcol_name0,
                    xset::name::panel3_detcol_name1,
                    xset::name::panel3_detcol_name2,
                    xset::name::panel3_detcol_name3,
                },
                {
                    xset::name::panel4_detcol_name0,
                    xset::name::panel4_detcol_name1,
                    xset::name::panel4_detcol_name2,
                    xset::name::panel4_detcol_name3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_size,
        {
            {
                xset::name::panel1_detcol_size,
                xset::name::panel2_detcol_size,
                xset::name::panel3_detcol_size,
                xset::name::panel4_detcol_size,
            },
            {{
                {
                    xset::name::panel1_detcol_size0,
                    xset::name::panel1_detcol_size1,
                    xset::name::panel1_detcol_size2,
                    xset::name::panel1_detcol_size3,
                },
                {
                    xset::name::panel2_detcol_size0,
                    xset::name::panel2_detcol_size1,
                    xset::name::panel2_detcol_size2,
                    xset::name::panel2_detcol_size3,
                },
                {
                    xset::name::panel3_detcol_size0,
                    xset::name::panel3_detcol_size1,
                    xset::name::panel3_detcol_size2,
                    xset::name::panel3_detcol_size3,
                },
                {
                    xset::name::panel4_detcol_size0,
                    xset::name::panel4_detcol_size1,
                    xset::name::panel4_detcol_size2,
                    xset::name::panel4_detcol_size3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_bytes,
        {
            {
                xset::name::panel1_detcol_bytes,
                xset::name::panel2_detcol_bytes,
                xset::name::panel3_detcol_bytes,
                xset::name::panel4_detcol_bytes,
            },
            {{
                {
                    xset::name::panel1_detcol_bytes0,
                    xset::name::panel1_detcol_bytes1,
                    xset::name::panel1_detcol_bytes2,
                    xset::name::panel1_detcol_bytes3,
                },
                {
                    xset::name::panel2_detcol_bytes0,
                    xset::name::panel2_detcol_bytes1,
                    xset::name::panel2_detcol_bytes2,
                    xset::name::panel2_detcol_bytes3,
                },
                {
                    xset::name::panel3_detcol_bytes0,
                    xset::name::panel3_detcol_bytes1,
                    xset::name::panel3_detcol_bytes2,
                    xset::name::panel3_detcol_bytes3,
                },
                {
                    xset::name::panel4_detcol_bytes0,
                    xset::name::panel4_detcol_bytes1,
                    xset::name::panel4_detcol_bytes2,
                    xset::name::panel4_detcol_bytes3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_type,
        {
            {
                xset::name::panel1_detcol_type,
                xset::name::panel2_detcol_type,
                xset::name::panel3_detcol_type,
                xset::name::panel4_detcol_type,
            },
            {{
                {
                    xset::name::panel1_detcol_type0,
                    xset::name::panel1_detcol_type1,
                    xset::name::panel1_detcol_type2,
                    xset::name::panel1_detcol_type3,
                },
                {
                    xset::name::panel2_detcol_type0,
                    xset::name::panel2_detcol_type1,
                    xset::name::panel2_detcol_type2,
                    xset::name::panel2_detcol_type3,
                },
                {
                    xset::name::panel3_detcol_type0,
                    xset::name::panel3_detcol_type1,
                    xset::name::panel3_detcol_type2,
                    xset::name::panel3_detcol_type3,
                },
                {
                    xset::name::panel4_detcol_type0,
                    xset::name::panel4_detcol_type1,
                    xset::name::panel4_detcol_type2,
                    xset::name::panel4_detcol_type3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_mime,
        {
            {
                xset::name::panel1_detcol_mime,
                xset::name::panel2_detcol_mime,
                xset::name::panel3_detcol_mime,
                xset::name::panel4_detcol_mime,
            },
            {{
                {
                    xset::name::panel1_detcol_mime0,
                    xset::name::panel1_detcol_mime1,
                    xset::name::panel1_detcol_mime2,
                    xset::name::panel1_detcol_mime3,
                },
                {
                    xset::name::panel2_detcol_mime0,
                    xset::name::panel2_detcol_mime1,
                    xset::name::panel2_detcol_mime2,
                    xset::name::panel2_detcol_mime3,
                },
                {
                    xset::name::panel3_detcol_mime0,
                    xset::name::panel3_detcol_mime1,
                    xset::name::panel3_detcol_mime2,
                    xset::name::panel3_detcol_mime3,
                },
                {
                    xset::name::panel4_detcol_mime0,
                    xset::name::panel4_detcol_mime1,
                    xset::name::panel4_detcol_mime2,
                    xset::name::panel4_detcol_mime3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_perm,
        {
            {
                xset::name::panel1_detcol_perm,
                xset::name::panel2_detcol_perm,
                xset::name::panel3_detcol_perm,
                xset::name::panel4_detcol_perm,
            },
            {{
                {
                    xset::name::panel1_detcol_perm0,
                    xset::name::panel1_detcol_perm1,
                    xset::name::panel1_detcol_perm2,
                    xset::name::panel1_detcol_perm3,
                },
                {
                    xset::name::panel2_detcol_perm0,
                    xset::name::panel2_detcol_perm1,
                    xset::name::panel2_detcol_perm2,
                    xset::name::panel2_detcol_perm3,
                },
                {
                    xset::name::panel3_detcol_perm0,
                    xset::name::panel3_detcol_perm1,
                    xset::name::panel3_detcol_perm2,
                    xset::name::panel3_detcol_perm3,
                },
                {
                    xset::name::panel4_detcol_perm0,
                    xset::name::panel4_detcol_perm1,
                    xset::name::panel4_detcol_perm2,
                    xset::name::panel4_detcol_perm3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_owner,
        {
            {
                xset::name::panel1_detcol_owner,
                xset::name::panel2_detcol_owner,
                xset::name::panel3_detcol_owner,
                xset::name::panel4_detcol_owner,
            },
            {{
                {
                    xset::name::panel1_detcol_owner0,
                    xset::name::panel1_detcol_owner1,
                    xset::name::panel1_detcol_owner2,
                    xset::name::panel1_detcol_owner3,
                },
                {
                    xset::name::panel2_detcol_owner0,
                    xset::name::panel2_detcol_owner1,
                    xset::name::panel2_detcol_owner2,
                    xset::name::panel2_detcol_owner3,
                },
                {
                    xset::name::panel3_detcol_owner0,
                    xset::name::panel3_detcol_owner1,
                    xset::name::panel3_detcol_owner2,
                    xset::name::panel3_detcol_owner3,
                },
                {
                    xset::name::panel4_detcol_owner0,
                    xset::name::panel4_detcol_owner1,
                    xset::name::panel4_detcol_owner2,
                    xset::name::panel4_detcol_owner3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_group,
        {
            {
                xset::name::panel1_detcol_group,
                xset::name::panel2_detcol_group,
                xset::name::panel3_detcol_group,
                xset::name::panel4_detcol_group,
            },
            {{
                {
                    xset::name::panel1_detcol_group0,
                    xset::name::panel1_detcol_group1,
                    xset::name::panel1_detcol_group2,
                    xset::name::panel1_detcol_group3,
                },
                {
                    xset::name::panel2_detcol_group0,
                    xset::name::panel2_detcol_group1,
                    xset::name::panel2_detcol_group2,
                    xset::name::panel2_detcol_group3,
                },
                {
                    xset::name::panel3_detcol_group0,
                    xset::name::panel3_detcol_group1,
                    xset::name::panel3_detcol_group2,
                    xset::name::panel3_detcol_group3,
                },
                {
                    xset::name::panel4_detcol_group0,
                    xset::name::panel4_detcol_group1,
                    xset::name::panel4_detcol_group2,
                    xset::name::panel4_detcol_group3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_atime,
        {
            {
                xset::name::panel1_detcol_atime,
                xset::name::panel2_detcol_atime,
                xset::name::panel3_detcol_atime,
                xset::name::panel4_detcol_atime,
            },
            {{
                {
                    xset::name::panel1_detcol_atime0,
                    xset::name::panel1_detcol_atime1,
                    xset::name::panel1_detcol_atime2,
                    xset::name::panel1_detcol_atime3,
                },
                {
                    xset::name::panel2_detcol_atime0,
                    xset::name::panel2_detcol_atime1,
                    xset::name::panel2_detcol_atime2,
                    xset::name::panel2_detcol_atime3,
                },
                {
                    xset::name::panel3_detcol_atime0,
                    xset::name::panel3_detcol_atime1,
                    xset::name::panel3_detcol_atime2,
                    xset::name::panel3_detcol_atime3,
                },
                {
                    xset::name::panel4_detcol_atime0,
                    xset::name::panel4_detcol_atime1,
                    xset::name::panel4_detcol_atime2,
                    xset::name::panel4_detcol_atime3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_btime,
        {
            {
                xset::name::panel1_detcol_btime,
                xset::name::panel2_detcol_btime,
                xset::name::panel3_detcol_btime,
                xset::name::panel4_detcol_btime,
            },
            {{
                {
                    xset::name::panel1_detcol_btime0,
                    xset::name::panel1_detcol_btime1,
                    xset::name::panel1_detcol_btime2,
                    xset::name::panel1_detcol_btime3,
                },
                {
                    xset::name::panel2_detcol_btime0,
                    xset::name::panel2_detcol_btime1,
                    xset::name::panel2_detcol_btime2,
                    xset::name::panel2_detcol_btime3,
                },
                {
                    xset::name::panel3_detcol_btime0,
                    xset::name::panel3_detcol_btime1,
                    xset::name::panel3_detcol_btime2,
                    xset::name::panel3_detcol_btime3,
                },
                {
                    xset::name::panel4_detcol_btime0,
                    xset::name::panel4_detcol_btime1,
                    xset::name::panel4_detcol_btime2,
                    xset::name::panel4_detcol_btime3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_ctime,
        {
            {
                xset::name::panel1_detcol_ctime,
                xset::name::panel2_detcol_ctime,
                xset::name::panel3_detcol_ctime,
                xset::name::panel4_detcol_ctime,
            },
            {{
                {
                    xset::name::panel1_detcol_ctime0,
                    xset::name::panel1_detcol_ctime1,
                    xset::name::panel1_detcol_ctime2,
                    xset::name::panel1_detcol_ctime3,
                },
                {
                    xset::name::panel2_detcol_ctime0,
                    xset::name::panel2_detcol_ctime1,
                    xset::name::panel2_detcol_ctime2,
                    xset::name::panel2_detcol_ctime3,
                },
                {
                    xset::name::panel3_detcol_ctime0,
                    xset::name::panel3_detcol_ctime1,
                    xset::name::panel3_detcol_ctime2,
                    xset::name::panel3_detcol_ctime3,
                },
                {
                    xset::name::panel4_detcol_ctime0,
                    xset::name::panel4_detcol_ctime1,
                    xset::name::panel4_detcol_ctime2,
                    xset::name::panel4_detcol_ctime3,
                },
            }},
        },
    },
    {
        xset::panel::detcol_mtime,
        {
            {
                xset::name::panel1_detcol_mtime,
                xset::name::panel2_detcol_mtime,
                xset::name::panel3_detcol_mtime,
                xset::name::panel4_detcol_mtime,
            },
            {{
                {
                    xset::name::panel1_detcol_mtime0,
                    xset::name::panel1_detcol_mtime1,
                    xset::name::panel1_detcol_mtime2,
                    xset::name::panel1_detcol_mtime3,
                },
                {
                    xset::name::panel2_detcol_mtime0,
                    xset::name::panel2_detcol_mtime1,
                    xset::name::panel2_detcol_mtime2,
                    xset::name::panel2_detcol_mtime3,
                },
                {
                    xset::name::panel3_detcol_mtime0,
                    xset::name::panel3_detcol_mtime1,
                    xset::name::panel3_detcol_mtime2,
                    xset::name::panel3_detcol_mtime3,
                },
                {
                    xset::name::panel4_detcol_mtime0,
                    xset::name::panel4_detcol_mtime1,
                    xset::name::panel4_detcol_mtime2,
                    xset::name::panel4_detcol_mtime3,
                },
            }},
        },
    },
    {
        xset::panel::sort_extra,
        {
            {
                xset::name::panel1_sort_extra,
                xset::name::panel2_sort_extra,
                xset::name::panel3_sort_extra,
                xset::name::panel4_sort_extra,
            },
            {{
                {
                    xset::name::panel1_sort_extra0,
                    xset::name::panel1_sort_extra1,
                    xset::name::panel1_sort_extra2,
                    xset::name::panel1_sort_extra3,
                },
                {
                    xset::name::panel2_sort_extra0,
                    xset::name::panel2_sort_extra1,
                    xset::name::panel2_sort_extra2,
                    xset::name::panel2_sort_extra3,
                },
                {
                    xset::name::panel3_sort_extra0,
                    xset::name::panel3_sort_extra1,
                    xset::name::panel3_sort_extra2,
                    xset::name::panel3_sort_extra3,
                },
                {
                    xset::name::panel4_sort_extra0,
                    xset::name::panel4_sort_extra1,
                    xset::name::panel4_sort_extra2,
                    xset::name::panel4_sort_extra3,
                },
            }},
        },
    },
    {
        xset::panel::book_fol,
        {
            {
                xset::name::panel1_book_fol,
                xset::name::panel2_book_fol,
                xset::name::panel3_book_fol,
                xset::name::panel4_book_fol,
            },
            {{
                {
                    xset::name::panel1_book_fol0,
                    xset::name::panel1_book_fol1,
                    xset::name::panel1_book_fol2,
                    xset::name::panel1_book_fol3,
                },
                {
                    xset::name::panel2_book_fol0,
                    xset::name::panel2_book_fol1,
                    xset::name::panel2_book_fol2,
                    xset::name::panel2_book_fol3,
                },
                {
                    xset::name::panel3_book_fol0,
                    xset::name::panel3_book_fol1,
                    xset::name::panel3_book_fol2,
                    xset::name::panel3_book_fol3,
                },
                {
                    xset::name::panel4_book_fol0,
                    xset::name::panel4_book_fol1,
                    xset::name::panel4_book_fol2,
                    xset::name::panel4_book_fol3,
                },
            }},
        },
    },
    {
        xset::panel::tool_l,
        {
            {
                xset::name::panel1_tool_l,
                xset::name::panel2_tool_l,
                xset::name::panel3_tool_l,
                xset::name::panel4_tool_l,
            },
            {{
                {
                    xset::name::panel1_tool_l0,
                    xset::name::panel1_tool_l1,
                    xset::name::panel1_tool_l2,
                    xset::name::panel1_tool_l3,
                },
                {
                    xset::name::panel2_tool_l0,
                    xset::name::panel2_tool_l1,
                    xset::name::panel2_tool_l2,
                    xset::name::panel2_tool_l3,
                },
                {
                    xset::name::panel3_tool_l0,
                    xset::name::panel3_tool_l1,
                    xset::name::panel3_tool_l2,
                    xset::name::panel3_tool_l3,
                },
                {
                    xset::name::panel4_tool_l0,
                    xset::name::panel4_tool_l1,
                    xset::name::panel4_tool_l2,
                    xset::name::panel4_tool_l3,
                },
            }},
        },
    },
    {
        xset::panel::tool_r,
        {
            {
                xset::name::panel1_tool_r,
                xset::name::panel2_tool_r,
                xset::name::panel3_tool_r,
                xset::name::panel4_tool_r,
            },
            {{
                {
                    xset::name::panel1_tool_r0,
                    xset::name::panel1_tool_r1,
                    xset::name::panel1_tool_r2,
                    xset::name::panel1_tool_r3,
                },
                {
                    xset::name::panel2_tool_r0,
                    xset::name::panel2_tool_r1,
                    xset::name::panel2_tool_r2,
                    xset::name::panel2_tool_r3,
                },
                {
                    xset::name::panel3_tool_r0,
                    xset::name::panel3_tool_r1,
                    xset::name::panel3_tool_r2,
                    xset::name::panel3_tool_r3,
                },
                {
                    xset::name::panel4_tool_r0,
                    xset::name::panel4_tool_r1,
                    xset::name::panel4_tool_r2,
                    xset::name::panel4_tool_r3,
                },
            }},
        },
    },
    {
        xset::panel::tool_s,
        {
            {
                xset::name::panel1_tool_s,
                xset::name::panel2_tool_s,
                xset::name::panel3_tool_s,
                xset::name::panel4_tool_s,
            },
            {{
                {
                    xset::name::panel1_tool_s0,
                    xset::name::panel1_tool_s1,
                    xset::name::panel1_tool_s2,
                    xset::name::panel1_tool_s3,
                },
                {
                    xset::name::panel2_tool_s0,
                    xset::name::panel2_tool_s1,
                    xset::name::panel2_tool_s2,
                    xset::name::panel2_tool_s3,
                },
                {
                    xset::name::panel3_tool_s0,
                    xset::name::panel3_tool_s1,
                    xset::name::panel3_tool_s2,
                    xset::name::panel3_tool_s3,
                },
                {
                    xset::name::panel4_tool_s0,
                    xset::name::panel4_tool_s1,
                    xset::name::panel4_tool_s2,
                    xset::name::panel4_tool_s3,
                },
            }},
        },
    },
};

// clang-format off

using namespace std::literals::string_view_literals;

static const std::unordered_map<xset::main_window_panel, std::string_view> main_window_panel_mode_map{
    {xset::main_window_panel::panel_neither, "0"sv},
    {xset::main_window_panel::panel_horiz,   "1"sv},
    {xset::main_window_panel::panel_vert,    "2"sv},
    {xset::main_window_panel::panel_both,    "3"sv},
};

// clang-format on

xset::name
xset::get_xsetname_from_name(const std::string_view name)
{
    const auto enum_value = magic_enum::enum_cast<xset::name>(name);
    if (!enum_value.has_value())
    {
        // ztd::logger::debug("name lookup custom {}", name);
        return xset::name::custom;
    }
    // ztd::logger::debug("name lookup {}", name);
    return enum_value.value();
}

const std::string_view
xset::get_name_from_xsetname(xset::name name)
{
    const auto value = magic_enum::enum_name(name);
    return value;
}

// panel

xset::name
xset::get_xsetname_from_panel(panel_t panel, xset::panel name)
{
    assert(panel == 1 || panel == 2 || panel == 3 || panel == 4);

    return xset_panel_lookup.at(name).panel[panel - 1];
}

const std::string_view
xset::get_name_from_panel(panel_t panel, xset::panel name)
{
    const auto set_name = xset::get_xsetname_from_panel(panel, name);
    const auto value = magic_enum::enum_name(set_name);
    return value;
}

// panel mode

xset::name
xset::get_xsetname_from_panel_mode(panel_t panel, xset::panel name, xset::main_window_panel mode)
{
    assert(panel == 1 || panel == 2 || panel == 3 || panel == 4);
    assert(name != xset::panel::show);

    return xset_panel_lookup.at(name).panel_mode[panel - 1][magic_enum::enum_integer(mode)];
}

const std::string_view
xset::get_name_from_panel_mode(panel_t panel, xset::panel name, xset::main_window_panel mode)
{
    const auto set_name = xset::get_xsetname_from_panel_mode(panel, name, mode);
    const auto value = magic_enum::enum_name(set_name);
    return value;
}

// main window panel mode

const std::string_view
xset::get_window_panel_mode(xset::main_window_panel mode)
{
    return main_window_panel_mode_map.at(mode);
}
