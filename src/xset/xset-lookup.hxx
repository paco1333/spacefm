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

#include "types.hxx"

namespace xset
{
    /**
     * These enum names are also used as the on-disk names and should not be changed.
     *
     * When adding new names ensure that MAGIC_ENUM_RANGE_MIN, and MAGIC_ENUM_RANGE_MAX
     * are large enough to hold the entire enum.
     */
    enum class name
    {
        // all non builtin xsets will the 'custom' value.
        custom,

        // separator
        separator,

        // dev menu
        dev_menu_remove,
        dev_menu_unmount,
        dev_menu_open,
        dev_menu_tab,
        dev_menu_mount,
        dev_menu_mark,
        dev_menu_settings,

        // dev settings
        dev_show,
        dev_show_internal_drives,
        dev_show_empty,
        dev_show_partition_tables,
        dev_show_net,
        dev_show_file,
        dev_ignore_udisks_hide,
        dev_show_hide_volumes,
        dev_dispname,

        dev_menu_auto,
        dev_automount_optical,
        dev_automount_removable,
        dev_ignore_udisks_nopolicy,
        dev_automount_volumes,
        dev_automount_dirs,
        dev_auto_open,
        dev_unmount_quit,

        dev_exec,
        dev_exec_fs,
        dev_exec_audio,
        dev_exec_video,
        dev_exec_insert,
        dev_exec_unmount,
        dev_exec_remove,

        dev_change,
        dev_fs_cnf,
        dev_net_cnf,

        // dev icons
        dev_icon,
        dev_icon_internal_mounted,
        dev_icon_internal_unmounted,
        dev_icon_remove_mounted,
        dev_icon_remove_unmounted,
        dev_icon_optical_mounted,
        dev_icon_optical_media,
        dev_icon_optical_nomedia,
        dev_icon_audiocd,
        dev_icon_floppy_mounted,
        dev_icon_floppy_unmounted,
        dev_icon_network,
        dev_icon_file,

        book_open,
        book_settings,
        book_icon,
        book_menu_icon,
        book_show,
        book_add,
        main_book,

        // Fonts
        font_general,
        font_view_icon,
        font_view_compact,

        // Rename/Move Dialog
        move_name,
        move_filename,
        move_parent,
        move_path,
        move_type,
        move_target,
        move_template,
        move_option,
        move_copy,
        move_link,
        move_copyt,
        move_linkt,
        move_as_root,
        move_dlg_help,
        move_dlg_confirm_create,

        // status bar
        status_middle,
        status_name,
        status_path,
        status_info,
        status_hide,

        // MAIN WINDOW MENUS //

        // File //
        main_new_window,
        main_search,
        main_terminal,
        main_save_session,
        main_save_tabs,
        main_exit,

        // VIEW //
        panel1_show,
        panel2_show,
        panel3_show,
        panel4_show,
        main_focus_panel,
        panel_prev,
        panel_next,
        panel_hide,
        panel_1,
        panel_2,
        panel_3,
        panel_4,

        main_auto,
        auto_inst,
        evt_start,
        evt_exit,
        auto_win,

        evt_win_new,
        evt_win_focus,
        evt_win_move,
        evt_win_click,
        evt_win_key,
        evt_win_close,

        auto_pnl,
        evt_pnl_focus,
        evt_pnl_show,
        evt_pnl_sel,

        auto_tab,
        evt_tab_new,
        evt_tab_chdir,
        evt_tab_focus,
        evt_tab_close,

        evt_device,
        main_title,
        main_icon,
        main_full,
        main_prefs,
        root_bar,
        view_thumb,

        // Help //
        main_about,
        main_dev,

        // Tasks //
        main_tasks,

        task_manager,

        task_columns,
        task_col_status,
        task_col_count,
        task_col_path,
        task_col_file,
        task_col_to,
        task_col_progress,
        task_col_total,
        task_col_started,
        task_col_elapsed,
        task_col_curspeed,
        task_col_curest,
        task_col_avgspeed,
        task_col_avgest,
        task_col_reorder,

        task_stop,
        task_pause,
        task_que,
        task_resume,
        task_showout,

        task_all,
        task_stop_all,
        task_pause_all,
        task_que_all,
        task_resume_all,

        task_show_manager,
        task_hide_manager,

        task_popups,
        task_pop_all,
        task_pop_top,
        task_pop_above,
        task_pop_stick,
        task_pop_detail,
        task_pop_over,
        task_pop_err,

        task_errors,
        task_err_first,
        task_err_any,
        task_err_cont,

        task_queue,
        task_q_new,
        task_q_smart,
        task_q_pause,

        // PANELS COMMON  //
        date_format,
        con_open,
        open_execute,
        open_edit,
        open_other,
        open_all,
        open_in_tab,

        opentab_new,
        opentab_prev,
        opentab_next,
        opentab_1,
        opentab_2,
        opentab_3,
        opentab_4,
        opentab_5,
        opentab_6,
        opentab_7,
        opentab_8,
        opentab_9,
        opentab_10,

        open_in_panel,
        open_in_panelprev,
        open_in_panelnext,
        open_in_panel1,
        open_in_panel2,
        open_in_panel3,
        open_in_panel4,

        archive_extract,
        archive_extract_to,
        archive_open,

        archive_default,
        archive_default_open_with_app,
        archive_default_extract,
        archive_default_extract_to,
        archive_default_open_with_archiver,

        open_new,
        new_file,
        new_directory,
        new_link,
        new_archive,
        tab_new,
        tab_new_here,
        new_bookmark,
        arc_dlg,

        new_app,
        con_go,

        go_refresh,
        go_back,
        go_forward,
        go_up,
        go_home,
        go_default,
        go_set_default,
        edit_canon,

        go_focus,
        focus_path_bar,
        focus_filelist,
        focus_dirtree,
        focus_book,
        focus_device,

        go_tab,
        tab_prev,
        tab_next,
        tab_restore,
        tab_close,
        tab_1,
        tab_2,
        tab_3,
        tab_4,
        tab_5,
        tab_6,
        tab_7,
        tab_8,
        tab_9,
        tab_10,

        con_view,
        view_list_style,
        view_columns,
        view_reorder_col,
        rubberband,

        view_sortby,
        sortby_name,
        sortby_size,
        sortby_bytes,
        sortby_type,
        sortby_mime,
        sortby_perm,
        sortby_owner,
        sortby_group,
        sortby_atime,
        sortby_btime,
        sortby_ctime,
        sortby_mtime,
        sortby_ascend,
        sortby_descend,
        sortx_alphanum,
        // sortx_natural,
        sortx_case,
        sortx_directories,
        sortx_files,
        sortx_mix,
        sortx_hidfirst,
        sortx_hidlast,

        view_refresh,
        path_seek,
        edit_cut,
        edit_copy,
        edit_paste,
        edit_rename,
        edit_delete,
        edit_trash,

        edit_submenu,
        copy_name,
        copy_parent,
        copy_path,
        paste_link,
        paste_target,
        paste_as,
        copy_to,
        copy_loc,
        copy_loc_last,

        copy_tab,
        copy_tab_prev,
        copy_tab_next,
        copy_tab_1,
        copy_tab_2,
        copy_tab_3,
        copy_tab_4,
        copy_tab_5,
        copy_tab_6,
        copy_tab_7,
        copy_tab_8,
        copy_tab_9,
        copy_tab_10,

        copy_panel,
        copy_panel_prev,
        copy_panel_next,
        copy_panel_1,
        copy_panel_2,
        copy_panel_3,
        copy_panel_4,

        move_to,
        move_loc,
        move_loc_last,

        move_tab,
        move_tab_prev,
        move_tab_next,
        move_tab_1,
        move_tab_2,
        move_tab_3,
        move_tab_4,
        move_tab_5,
        move_tab_6,
        move_tab_7,
        move_tab_8,
        move_tab_9,
        move_tab_10,

        move_panel,
        move_panel_prev,
        move_panel_next,
        move_panel_1,
        move_panel_2,
        move_panel_3,
        move_panel_4,

        edit_hide,
        select_all,
        select_un,
        select_invert,
        select_patt,

        // Properties //
        con_prop,
        prop_info,
        prop_perm,
        prop_quick,

        perm_r,
        perm_rw,
        perm_rwx,
        perm_r_r,
        perm_rw_r,
        perm_rw_rw,
        perm_rwxr_x,
        perm_rwxrwx,
        perm_r_r_r,
        perm_rw_r_r,
        perm_rw_rw_rw,
        perm_rwxr_r,
        perm_rwxr_xr_x,
        perm_rwxrwxrwx,
        perm_rwxrwxrwt,
        perm_unstick,
        perm_stick,

        perm_recurs,
        perm_go_w,
        perm_go_rwx,
        perm_ugo_w,
        perm_ugo_rx,
        perm_ugo_rwx,

        prop_root,
        rperm_rw,
        rperm_rwx,
        rperm_rw_r,
        rperm_rw_rw,
        rperm_rwxr_x,
        rperm_rwxrwx,
        rperm_rw_r_r,
        rperm_rw_rw_rw,
        rperm_rwxr_r,
        rperm_rwxr_xr_x,
        rperm_rwxrwxrwx,
        rperm_rwxrwxrwt,
        rperm_unstick,
        rperm_stick,

        rperm_recurs,
        rperm_go_w,
        rperm_go_rwx,
        rperm_ugo_w,
        rperm_ugo_rx,
        rperm_ugo_rwx,

        rperm_own,
        own_myuser,
        own_myuser_users,
        own_user1,
        own_user1_users,
        own_user2,
        own_user2_users,
        own_root,
        own_root_users,
        own_root_myuser,
        own_root_user1,
        own_root_user2,

        own_recurs,
        rown_myuser,
        rown_myuser_users,
        rown_user1,
        rown_user1_users,
        rown_user2,
        rown_user2_users,
        rown_root,
        rown_root_users,
        rown_root_myuser,
        rown_root_user1,
        rown_root_user2,

        // PANELS //
        panel_sliders,

        // panel1
        panel1_show_toolbox,
        panel1_show_devmon,
        panel1_show_dirtree,
        panel1_show_sidebar,
        panel1_slider_positions,
        panel1_list_detailed,
        panel1_list_icons,
        panel1_list_compact,
        panel1_list_large,
        panel1_show_hidden,
        panel1_icon_tab,
        panel1_icon_status,
        panel1_detcol_name,
        panel1_detcol_size,
        panel1_detcol_bytes,
        panel1_detcol_type,
        panel1_detcol_mime,
        panel1_detcol_perm,
        panel1_detcol_owner,
        panel1_detcol_group,
        panel1_detcol_atime,
        panel1_detcol_btime,
        panel1_detcol_ctime,
        panel1_detcol_mtime,
        panel1_sort_extra,
        panel1_book_fol,
        panel1_tool_l,
        panel1_tool_r,
        panel1_tool_s,

        // panel2
        panel2_show_toolbox,
        panel2_show_devmon,
        panel2_show_dirtree,
        panel2_show_sidebar,
        panel2_slider_positions,
        panel2_list_detailed,
        panel2_list_icons,
        panel2_list_compact,
        panel2_list_large,
        panel2_show_hidden,
        panel2_icon_tab,
        panel2_icon_status,
        panel2_detcol_name,
        panel2_detcol_size,
        panel2_detcol_bytes,
        panel2_detcol_type,
        panel2_detcol_mime,
        panel2_detcol_perm,
        panel2_detcol_owner,
        panel2_detcol_group,
        panel2_detcol_atime,
        panel2_detcol_btime,
        panel2_detcol_ctime,
        panel2_detcol_mtime,
        panel2_sort_extra,
        panel2_book_fol,
        panel2_tool_l,
        panel2_tool_r,
        panel2_tool_s,

        // panel3
        panel3_show_toolbox,
        panel3_show_devmon,
        panel3_show_dirtree,
        panel3_show_sidebar,
        panel3_slider_positions,
        panel3_list_detailed,
        panel3_list_icons,
        panel3_list_compact,
        panel3_list_large,
        panel3_show_hidden,
        panel3_icon_tab,
        panel3_icon_status,
        panel3_detcol_name,
        panel3_detcol_size,
        panel3_detcol_bytes,
        panel3_detcol_type,
        panel3_detcol_mime,
        panel3_detcol_perm,
        panel3_detcol_owner,
        panel3_detcol_group,
        panel3_detcol_atime,
        panel3_detcol_btime,
        panel3_detcol_ctime,
        panel3_detcol_mtime,
        panel3_sort_extra,
        panel3_book_fol,
        panel3_tool_l,
        panel3_tool_r,
        panel3_tool_s,

        // panel4
        panel4_show_toolbox,
        panel4_show_devmon,
        panel4_show_dirtree,
        panel4_show_sidebar,
        panel4_slider_positions,
        panel4_list_detailed,
        panel4_list_icons,
        panel4_list_compact,
        panel4_list_large,
        panel4_show_hidden,
        panel4_icon_tab,
        panel4_icon_status,
        panel4_detcol_name,
        panel4_detcol_size,
        panel4_detcol_bytes,
        panel4_detcol_type,
        panel4_detcol_mime,
        panel4_detcol_perm,
        panel4_detcol_owner,
        panel4_detcol_group,
        panel4_detcol_atime,
        panel4_detcol_btime,
        panel4_detcol_ctime,
        panel4_detcol_mtime,
        panel4_sort_extra,
        panel4_book_fol,
        panel4_tool_l,
        panel4_tool_r,
        panel4_tool_s,

        // panel modes

        // panel1

        // panel1 mode 0
        panel1_show_toolbox0,
        panel1_show_devmon0,
        panel1_show_dirtree0,
        panel1_show_sidebar0,
        panel1_slider_positions0,
        panel1_list_detailed0,
        panel1_list_icons0,
        panel1_list_compact0,
        panel1_list_large0,
        panel1_show_hidden0,
        panel1_icon_tab0,
        panel1_icon_status0,
        panel1_detcol_name0,
        panel1_detcol_size0,
        panel1_detcol_bytes0,
        panel1_detcol_type0,
        panel1_detcol_mime0,
        panel1_detcol_perm0,
        panel1_detcol_owner0,
        panel1_detcol_group0,
        panel1_detcol_atime0,
        panel1_detcol_btime0,
        panel1_detcol_ctime0,
        panel1_detcol_mtime0,
        panel1_sort_extra0,
        panel1_book_fol0,
        panel1_tool_l0,
        panel1_tool_r0,
        panel1_tool_s0,

        // panel1 mode 1
        panel1_show_toolbox1,
        panel1_show_devmon1,
        panel1_show_dirtree1,
        panel1_show_sidebar1,
        panel1_slider_positions1,
        panel1_list_detailed1,
        panel1_list_icons1,
        panel1_list_compact1,
        panel1_list_large1,
        panel1_show_hidden1,
        panel1_icon_tab1,
        panel1_icon_status1,
        panel1_detcol_name1,
        panel1_detcol_size1,
        panel1_detcol_bytes1,
        panel1_detcol_type1,
        panel1_detcol_mime1,
        panel1_detcol_perm1,
        panel1_detcol_owner1,
        panel1_detcol_group1,
        panel1_detcol_atime1,
        panel1_detcol_btime1,
        panel1_detcol_ctime1,
        panel1_detcol_mtime1,
        panel1_sort_extra1,
        panel1_book_fol1,
        panel1_tool_l1,
        panel1_tool_r1,
        panel1_tool_s1,

        // panel1 mode 2
        panel1_show_toolbox2,
        panel1_show_devmon2,
        panel1_show_dirtree2,
        panel1_show_sidebar2,
        panel1_slider_positions2,
        panel1_list_detailed2,
        panel1_list_icons2,
        panel1_list_compact2,
        panel1_list_large2,
        panel1_show_hidden2,
        panel1_icon_tab2,
        panel1_icon_status2,
        panel1_detcol_name2,
        panel1_detcol_size2,
        panel1_detcol_bytes2,
        panel1_detcol_type2,
        panel1_detcol_mime2,
        panel1_detcol_perm2,
        panel1_detcol_owner2,
        panel1_detcol_group2,
        panel1_detcol_atime2,
        panel1_detcol_btime2,
        panel1_detcol_ctime2,
        panel1_detcol_mtime2,
        panel1_sort_extra2,
        panel1_book_fol2,
        panel1_tool_l2,
        panel1_tool_r2,
        panel1_tool_s2,

        // panel1 mode 3
        panel1_show_toolbox3,
        panel1_show_devmon3,
        panel1_show_dirtree3,
        panel1_show_sidebar3,
        panel1_slider_positions3,
        panel1_list_detailed3,
        panel1_list_icons3,
        panel1_list_compact3,
        panel1_list_large3,
        panel1_show_hidden3,
        panel1_icon_tab3,
        panel1_icon_status3,
        panel1_detcol_name3,
        panel1_detcol_size3,
        panel1_detcol_bytes3,
        panel1_detcol_type3,
        panel1_detcol_mime3,
        panel1_detcol_perm3,
        panel1_detcol_owner3,
        panel1_detcol_group3,
        panel1_detcol_atime3,
        panel1_detcol_btime3,
        panel1_detcol_ctime3,
        panel1_detcol_mtime3,
        panel1_sort_extra3,
        panel1_book_fol3,
        panel1_tool_l3,
        panel1_tool_r3,
        panel1_tool_s3,

        // panel2

        // panel2 mode 0
        panel2_show_toolbox0,
        panel2_show_devmon0,
        panel2_show_dirtree0,
        panel2_show_sidebar0,
        panel2_slider_positions0,
        panel2_list_detailed0,
        panel2_list_icons0,
        panel2_list_compact0,
        panel2_list_large0,
        panel2_show_hidden0,
        panel2_icon_tab0,
        panel2_icon_status0,
        panel2_detcol_name0,
        panel2_detcol_size0,
        panel2_detcol_bytes0,
        panel2_detcol_type0,
        panel2_detcol_mime0,
        panel2_detcol_perm0,
        panel2_detcol_owner0,
        panel2_detcol_group0,
        panel2_detcol_atime0,
        panel2_detcol_btime0,
        panel2_detcol_ctime0,
        panel2_detcol_mtime0,
        panel2_sort_extra0,
        panel2_book_fol0,
        panel2_tool_l0,
        panel2_tool_r0,
        panel2_tool_s0,

        // panel2 mode 1
        panel2_show_toolbox1,
        panel2_show_devmon1,
        panel2_show_dirtree1,
        panel2_show_sidebar1,
        panel2_slider_positions1,
        panel2_list_detailed1,
        panel2_list_icons1,
        panel2_list_compact1,
        panel2_list_large1,
        panel2_show_hidden1,
        panel2_icon_tab1,
        panel2_icon_status1,
        panel2_detcol_name1,
        panel2_detcol_size1,
        panel2_detcol_bytes1,
        panel2_detcol_type1,
        panel2_detcol_mime1,
        panel2_detcol_perm1,
        panel2_detcol_owner1,
        panel2_detcol_group1,
        panel2_detcol_atime1,
        panel2_detcol_btime1,
        panel2_detcol_ctime1,
        panel2_detcol_mtime1,
        panel2_sort_extra1,
        panel2_book_fol1,
        panel2_tool_l1,
        panel2_tool_r1,
        panel2_tool_s1,

        // panel2 mode 2
        panel2_show_toolbox2,
        panel2_show_devmon2,
        panel2_show_dirtree2,
        panel2_show_sidebar2,
        panel2_slider_positions2,
        panel2_list_detailed2,
        panel2_list_icons2,
        panel2_list_compact2,
        panel2_list_large2,
        panel2_show_hidden2,
        panel2_icon_tab2,
        panel2_icon_status2,
        panel2_detcol_name2,
        panel2_detcol_size2,
        panel2_detcol_bytes2,
        panel2_detcol_type2,
        panel2_detcol_mime2,
        panel2_detcol_perm2,
        panel2_detcol_owner2,
        panel2_detcol_group2,
        panel2_detcol_atime2,
        panel2_detcol_btime2,
        panel2_detcol_ctime2,
        panel2_detcol_mtime2,
        panel2_sort_extra2,
        panel2_book_fol2,
        panel2_tool_l2,
        panel2_tool_r2,
        panel2_tool_s2,

        // panel2 mode 3
        panel2_show_toolbox3,
        panel2_show_devmon3,
        panel2_show_dirtree3,
        panel2_show_sidebar3,
        panel2_slider_positions3,
        panel2_list_detailed3,
        panel2_list_icons3,
        panel2_list_compact3,
        panel2_list_large3,
        panel2_show_hidden3,
        panel2_icon_tab3,
        panel2_icon_status3,
        panel2_detcol_name3,
        panel2_detcol_size3,
        panel2_detcol_bytes3,
        panel2_detcol_type3,
        panel2_detcol_mime3,
        panel2_detcol_perm3,
        panel2_detcol_owner3,
        panel2_detcol_group3,
        panel2_detcol_atime3,
        panel2_detcol_btime3,
        panel2_detcol_ctime3,
        panel2_detcol_mtime3,
        panel2_sort_extra3,
        panel2_book_fol3,
        panel2_tool_l3,
        panel2_tool_r3,
        panel2_tool_s3,

        // panel3

        // panel3 mode 0
        panel3_show_toolbox0,
        panel3_show_devmon0,
        panel3_show_dirtree0,
        panel3_show_sidebar0,
        panel3_slider_positions0,
        panel3_list_detailed0,
        panel3_list_icons0,
        panel3_list_compact0,
        panel3_list_large0,
        panel3_show_hidden0,
        panel3_icon_tab0,
        panel3_icon_status0,
        panel3_detcol_name0,
        panel3_detcol_size0,
        panel3_detcol_bytes0,
        panel3_detcol_type0,
        panel3_detcol_mime0,
        panel3_detcol_perm0,
        panel3_detcol_owner0,
        panel3_detcol_group0,
        panel3_detcol_atime0,
        panel3_detcol_btime0,
        panel3_detcol_ctime0,
        panel3_detcol_mtime0,
        panel3_sort_extra0,
        panel3_book_fol0,
        panel3_tool_l0,
        panel3_tool_r0,
        panel3_tool_s0,

        // panel3 mode 1
        panel3_show_toolbox1,
        panel3_show_devmon1,
        panel3_show_dirtree1,
        panel3_show_sidebar1,
        panel3_slider_positions1,
        panel3_list_detailed1,
        panel3_list_icons1,
        panel3_list_compact1,
        panel3_list_large1,
        panel3_show_hidden1,
        panel3_icon_tab1,
        panel3_icon_status1,
        panel3_detcol_name1,
        panel3_detcol_size1,
        panel3_detcol_bytes1,
        panel3_detcol_type1,
        panel3_detcol_mime1,
        panel3_detcol_perm1,
        panel3_detcol_owner1,
        panel3_detcol_group1,
        panel3_detcol_atime1,
        panel3_detcol_btime1,
        panel3_detcol_ctime1,
        panel3_detcol_mtime1,
        panel3_sort_extra1,
        panel3_book_fol1,
        panel3_tool_l1,
        panel3_tool_r1,
        panel3_tool_s1,

        // panel3 mode 2
        panel3_show_toolbox2,
        panel3_show_devmon2,
        panel3_show_dirtree2,
        panel3_show_sidebar2,
        panel3_slider_positions2,
        panel3_list_detailed2,
        panel3_list_icons2,
        panel3_list_compact2,
        panel3_list_large2,
        panel3_show_hidden2,
        panel3_icon_tab2,
        panel3_icon_status2,
        panel3_detcol_name2,
        panel3_detcol_size2,
        panel3_detcol_bytes2,
        panel3_detcol_type2,
        panel3_detcol_mime2,
        panel3_detcol_perm2,
        panel3_detcol_owner2,
        panel3_detcol_group2,
        panel3_detcol_atime2,
        panel3_detcol_btime2,
        panel3_detcol_ctime2,
        panel3_detcol_mtime2,
        panel3_sort_extra2,
        panel3_book_fol2,
        panel3_tool_l2,
        panel3_tool_r2,
        panel3_tool_s2,

        // panel3 mode 3
        panel3_show_toolbox3,
        panel3_show_devmon3,
        panel3_show_dirtree3,
        panel3_show_sidebar3,
        panel3_slider_positions3,
        panel3_list_detailed3,
        panel3_list_icons3,
        panel3_list_compact3,
        panel3_list_large3,
        panel3_show_hidden3,
        panel3_icon_tab3,
        panel3_icon_status3,
        panel3_detcol_name3,
        panel3_detcol_size3,
        panel3_detcol_bytes3,
        panel3_detcol_type3,
        panel3_detcol_mime3,
        panel3_detcol_perm3,
        panel3_detcol_owner3,
        panel3_detcol_group3,
        panel3_detcol_atime3,
        panel3_detcol_btime3,
        panel3_detcol_ctime3,
        panel3_detcol_mtime3,
        panel3_sort_extra3,
        panel3_book_fol3,
        panel3_tool_l3,
        panel3_tool_r3,
        panel3_tool_s3,

        // panel4

        // panel4 mode 0
        panel4_show_toolbox0,
        panel4_show_devmon0,
        panel4_show_dirtree0,
        panel4_show_sidebar0,
        panel4_slider_positions0,
        panel4_list_detailed0,
        panel4_list_icons0,
        panel4_list_compact0,
        panel4_list_large0,
        panel4_show_hidden0,
        panel4_icon_tab0,
        panel4_icon_status0,
        panel4_detcol_name0,
        panel4_detcol_size0,
        panel4_detcol_bytes0,
        panel4_detcol_type0,
        panel4_detcol_mime0,
        panel4_detcol_perm0,
        panel4_detcol_owner0,
        panel4_detcol_group0,
        panel4_detcol_atime0,
        panel4_detcol_btime0,
        panel4_detcol_ctime0,
        panel4_detcol_mtime0,
        panel4_sort_extra0,
        panel4_book_fol0,
        panel4_tool_l0,
        panel4_tool_r0,
        panel4_tool_s0,

        // panel4 mode 1
        panel4_show_toolbox1,
        panel4_show_devmon1,
        panel4_show_dirtree1,
        panel4_show_sidebar1,
        panel4_slider_positions1,
        panel4_list_detailed1,
        panel4_list_icons1,
        panel4_list_compact1,
        panel4_list_large1,
        panel4_show_hidden1,
        panel4_icon_tab1,
        panel4_icon_status1,
        panel4_detcol_name1,
        panel4_detcol_size1,
        panel4_detcol_bytes1,
        panel4_detcol_type1,
        panel4_detcol_mime1,
        panel4_detcol_perm1,
        panel4_detcol_owner1,
        panel4_detcol_group1,
        panel4_detcol_atime1,
        panel4_detcol_btime1,
        panel4_detcol_ctime1,
        panel4_detcol_mtime1,
        panel4_sort_extra1,
        panel4_book_fol1,
        panel4_tool_l1,
        panel4_tool_r1,
        panel4_tool_s1,

        // panel4 mode 2
        panel4_show_toolbox2,
        panel4_show_devmon2,
        panel4_show_dirtree2,
        panel4_show_sidebar2,
        panel4_slider_positions2,
        panel4_list_detailed2,
        panel4_list_icons2,
        panel4_list_compact2,
        panel4_list_large2,
        panel4_show_hidden2,
        panel4_icon_tab2,
        panel4_icon_status2,
        panel4_detcol_name2,
        panel4_detcol_size2,
        panel4_detcol_bytes2,
        panel4_detcol_type2,
        panel4_detcol_mime2,
        panel4_detcol_perm2,
        panel4_detcol_owner2,
        panel4_detcol_group2,
        panel4_detcol_atime2,
        panel4_detcol_btime2,
        panel4_detcol_ctime2,
        panel4_detcol_mtime2,
        panel4_sort_extra2,
        panel4_book_fol2,
        panel4_tool_l2,
        panel4_tool_r2,
        panel4_tool_s2,

        // panel4 mode 3
        panel4_show_toolbox3,
        panel4_show_devmon3,
        panel4_show_dirtree3,
        panel4_show_sidebar3,
        panel4_slider_positions3,
        panel4_list_detailed3,
        panel4_list_icons3,
        panel4_list_compact3,
        panel4_list_large3,
        panel4_show_hidden3,
        panel4_icon_tab3,
        panel4_icon_status3,
        panel4_detcol_name3,
        panel4_detcol_size3,
        panel4_detcol_bytes3,
        panel4_detcol_type3,
        panel4_detcol_mime3,
        panel4_detcol_perm3,
        panel4_detcol_owner3,
        panel4_detcol_group3,
        panel4_detcol_atime3,
        panel4_detcol_btime3,
        panel4_detcol_ctime3,
        panel4_detcol_mtime3,
        panel4_sort_extra3,
        panel4_book_fol3,
        panel4_tool_l3,
        panel4_tool_r3,
        panel4_tool_s3,

        // speed
        book_newtab,
        book_single,
        dev_newtab,
        dev_single,

        // dialog
        app_dlg,
        context_dlg,
        file_dlg,
        text_dlg,

        // other
        config_version,
        book_new,
        drag_action,
        editor,
        root_editor,
        su_command,

        // HANDLERS //

        // handlers file
        handler_file_iso,

        // handlers fs
        handler_filesystem_default,
        handler_filesystem_fuseiso,
        handler_filesystem_udiso,

        // handlers net
        handler_network_ftp,
        handler_network_fuse,
        handler_network_fusesmb,
        handler_network_gphoto,
        handler_network_http,
        handler_network_ifuse,
        handler_network_mtp,
        handler_network_ssh,
        handler_network_udevil,
        handler_network_udevilsmb,

        // other
        tool_l,
        tool_r,
        tool_s,
    };

    enum class panel
    {
        show,
        show_toolbox,
        show_devmon,
        show_dirtree,
        show_sidebar,
        slider_positions,
        list_detailed,
        list_icons,
        list_compact,
        list_large,
        show_hidden,
        icon_tab,
        icon_status,
        detcol_name,
        detcol_size,
        detcol_bytes,
        detcol_type,
        detcol_mime,
        detcol_perm,
        detcol_owner,
        detcol_group,
        detcol_atime,
        detcol_btime,
        detcol_ctime,
        detcol_mtime,
        sort_extra,
        book_fol,
        tool_l,
        tool_r,
        tool_s,
    };

    enum class var
    {
        s,
        b,
        x,
        y,
        z,
        key,
        keymod,
        style,
        desc,
        title,
        menu_label,
        menu_label_custom,
        icn,
        icon,
        shared_key,
        next,
        prev,
        parent,
        child,
        context,
        line,
        tool,
        task,
        task_pop,
        task_err,
        task_out,
        run_in_terminal,
        keep_terminal,
        scroll_lock,
        disable,
        opener,
    };

    enum class main_window_panel
    { // how a panel shares vertical and horizontal space with other panels
        panel_neither,
        panel_horiz,
        panel_vert,
        panel_both
    };

    xset::name get_xsetname_from_name(const std::string_view name);
    const std::string_view get_name_from_xsetname(xset::name name);

    xset::name get_xsetname_from_panel(panel_t panel, xset::panel panel_var);
    const std::string_view get_name_from_panel(panel_t panel, xset::panel name);

    xset::name get_xsetname_from_panel_mode(panel_t panel, xset::panel name,
                                            xset::main_window_panel mode);
    const std::string_view get_name_from_panel_mode(panel_t panel, xset::panel name,
                                                    xset::main_window_panel mode);

    xset::var get_xsetvar_from_name(const std::string_view name);
    const std::string_view get_name_from_xsetvar(xset::var name);

    // main window panel mode
    const std::string_view get_window_panel_mode(xset::main_window_panel mode);
} // namespace xset
