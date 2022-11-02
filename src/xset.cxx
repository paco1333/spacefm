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

#include <vector>

#include <type_traits>

#include <fmt/format.h>

#include <glib.h>

#include <ztd/ztd.hxx>
#include <ztd/ztd_logger.hxx>

#include "types.hxx"

#include "xset.hxx"

std::vector<xset_t> xsets;

XSet::XSet(const std::string& name, XSetName xset_name)
{
    // LOG_INFO("XSet Constructor");

    this->name = ztd::strdup(name);
    this->xset_name = xset_name;
}

XSet::~XSet()
{
    // LOG_INFO("XSet Destructor");

    if (this->name)
        free(this->name);
    if (this->s)
        free(this->s);
    if (this->x)
        free(this->x);
    if (this->y)
        free(this->y);
    if (this->z)
        free(this->z);
    if (this->menu_label)
        free(this->menu_label);
    if (this->shared_key)
        free(this->shared_key);
    if (this->icon)
        free(this->icon);
    if (this->desc)
        free(this->desc);
    if (this->title)
        free(this->title);
    if (this->next)
        free(this->next);
    if (this->parent)
        free(this->parent);
    if (this->child)
        free(this->child);
    if (this->prev)
        free(this->prev);
    if (this->line)
        free(this->line);
    if (this->context)
        free(this->context);
    if (this->plugin)
    {
        if (this->plug_dir)
            free(this->plug_dir);
        if (this->plug_name)
            free(this->plug_name);
    }
}

const char*
XSet::get_name() const noexcept
{
    return this->name;
}

bool
XSet::is_xset_name(XSetName val) noexcept
{
    return (this->xset_name == val);
}

bool
XSet::is_xset_name(const std::vector<XSetName>& val) noexcept
{
    for (XSetName v: val)
    {
        if (this->xset_name == v)
            return true;
    }
    return false;
}

XSetName
XSet::get_xset_name() const noexcept
{
    return this->xset_name;
}

bool
XSet::is_b(XSetB bval) noexcept
{
    return (this->b == bval);
}

bool
XSet::get_b() const noexcept
{
    return (this->b == XSetB::XSET_B_TRUE);
}

void
XSet::set_b(bool bval) noexcept
{
    this->b = (bval ? XSetB::XSET_B_TRUE : XSetB::XSET_B_FALSE);
}

void
XSet::set_b(XSetB bval) noexcept
{
    this->b = bval;
}

char*
XSet::get_s() const noexcept
{
    return this->s;
}

int
XSet::get_s_int() const noexcept
{
    if (!this->s)
        return 0;
    return std::stoi(this->s);
}

void
XSet::set_s(const char* val) noexcept
{
    if (this->s)
        free(this->s);
    this->s = ztd::strdup(val);
}

void
XSet::set_s(const std::string& val) noexcept
{
    if (this->s)
        free(this->s);
    this->s = ztd::strdup(val);
}

char*
XSet::get_x() const noexcept
{
    return this->x;
}

int
XSet::get_x_int() const noexcept
{
    if (!this->x)
        return 0;
    return std::stoi(this->x);
}

void
XSet::set_x(const char* val) noexcept
{
    if (this->x)
        free(this->x);
    this->x = ztd::strdup(val);
}

void
XSet::set_x(const std::string& val) noexcept
{
    if (this->x)
        free(this->x);
    this->x = ztd::strdup(val);
}

char*
XSet::get_y() const noexcept
{
    return this->y;
}

int
XSet::get_y_int() const noexcept
{
    if (!this->y)
        return 0;
    return std::stoi(this->y);
}

void
XSet::set_y(const char* val) noexcept
{
    if (this->y)
        free(this->y);
    this->y = ztd::strdup(val);
}

void
XSet::set_y(const std::string& val) noexcept
{
    if (this->y)
        free(this->y);
    this->y = ztd::strdup(val);
}

char*
XSet::get_z() const noexcept
{
    return this->z;
}

int
XSet::get_z_int() const noexcept
{
    if (!this->z)
        return 0;
    return std::stoi(this->z);
}

void
XSet::set_z(const char* val) noexcept
{
    if (this->z)
        free(this->z);
    this->z = ztd::strdup(val);
}

void
XSet::set_z(const std::string& val) noexcept
{
    if (this->z)
        free(this->z);
    this->z = ztd::strdup(val);
}

bool
XSet::get_disable() const noexcept
{
    return this->disable;
}

void
XSet::set_disable(bool bval) noexcept
{
    this->disable = bval;
}

char*
XSet::get_menu_label() const noexcept
{
    return this->menu_label;
}

void
XSet::set_menu_label(const char* val) noexcept
{
    if (this->menu_label)
        free(this->menu_label);
    this->menu_label = ztd::strdup(val);
    if (this->lock)
    { // indicate that menu label is not default and should be saved
        this->set_in_terminal(true);
    }
}

void
XSet::set_menu_label(const std::string& val) noexcept
{
    if (this->menu_label)
        free(this->menu_label);
    this->menu_label = ztd::strdup(val);
    if (this->lock)
    { // indicate that menu label is not default and should be saved
        this->set_in_terminal(true);
    }
}

void
XSet::set_menu_label_custom(const char* val) noexcept
{
    if (!this->lock || !ztd::same(this->menu_label, val))
        set_menu_label(val);
}

void
XSet::set_menu_label_custom(const std::string& val) noexcept
{
    if (!this->lock || !ztd::same(this->menu_label, val))
        set_menu_label(val);
}

bool
XSet::is_menu_style(XSetMenu val) noexcept
{
    return (this->menu_style == val);
}

bool
XSet::is_menu_style(const std::vector<XSetMenu>& val) noexcept
{
    for (XSetMenu v: val)
    {
        if (this->menu_style == v)
            return true;
    }
    return false;
}

XSetMenu
XSet::get_menu_style() const noexcept
{
    return this->menu_style;
}

void
XSet::set_menu_style(XSetMenu val) noexcept
{
    this->menu_style = val;
}

void
XSet::set_cb(GFunc func, void* data) noexcept
{
    this->cb_func = func;
    this->cb_data = data;
}

void
XSet::set_ob1(const char* ob, void* data) noexcept
{
    if (this->ob1)
        free(this->ob1);
    this->ob1 = ztd::strdup(ob);
    this->ob1_data = data;
}

void
XSet::set_ob1(const char* ob, const char* data) noexcept
{
    if (this->ob1)
        free(this->ob1);
    this->ob1 = ztd::strdup(ob);
    // this->ob1_data = const_cast<char*>(data);
    this->ob1_data = ztd::strdup(data);
}

void
XSet::set_ob1_int(const char* ob, int data) noexcept
{
    if (this->ob1)
        free(this->ob1);
    this->ob1 = ztd::strdup(ob);
    this->ob1_data = GINT_TO_POINTER(data);
}

void
XSet::set_ob2(const char* ob, void* data) noexcept
{
    if (this->ob2)
        free(this->ob2);
    this->ob2 = ztd::strdup(ob);
    this->ob2_data = data;
}

unsigned int
XSet::get_key() const noexcept
{
    return this->key;
}

void
XSet::set_key(unsigned int val) noexcept
{
    this->key = val;
}

unsigned int
XSet::get_keymod() const noexcept
{
    return this->keymod;
}

void
XSet::set_keymod(unsigned int val) noexcept
{
    this->keymod = val;
}

char*
XSet::get_shared_key() const noexcept
{
    return this->shared_key;
}

void
XSet::set_shared_key(const char* val) noexcept
{
    if (this->shared_key)
        free(this->shared_key);
    this->shared_key = ztd::strdup(val);
}

void
XSet::set_shared_key(const std::string& val) noexcept
{
    if (this->shared_key)
        free(this->shared_key);
    this->shared_key = ztd::strdup(val);
}

char*
XSet::get_icon() const noexcept
{
    return this->icon;
}

void
XSet::set_icon(const char* val) noexcept
{
    // icn is only used >= 0.9.0 for changed lock default icon
    if (this->icon)
        free(this->icon);
    this->icon = ztd::strdup(val);
    if (this->lock)
    { // indicate that icon is not default and should be saved
        this->keep_terminal = true;
    }
}

void
XSet::set_icon(const std::string& val) noexcept
{
    // icn is only used >= 0.9.0 for changed lock default icon
    if (this->icon)
        free(this->icon);
    this->icon = ztd::strdup(val);
    if (this->lock)
    { // indicate that icon is not default and should be saved
        this->keep_terminal = true;
    }
}

char*
XSet::get_desc() const noexcept
{
    return this->desc;
}

void
XSet::set_desc(const char* val) noexcept
{
    if (this->desc)
        free(this->desc);
    this->desc = ztd::strdup(val);
}

void
XSet::set_desc(const std::string& val) noexcept
{
    if (this->desc)
        free(this->desc);
    this->desc = ztd::strdup(val);
}

char*
XSet::get_title() const noexcept
{
    return this->title;
}

void
XSet::set_title(const char* val) noexcept
{
    if (this->title)
        free(this->title);
    this->title = ztd::strdup(val);
}

void
XSet::set_title(const std::string& val) noexcept
{
    if (this->title)
        free(this->title);
    this->title = ztd::strdup(val);
}

char*
XSet::get_next() const noexcept
{
    return this->next;
}

void
XSet::set_next(const char* val) noexcept
{
    if (this->next)
        free(this->next);
    this->next = ztd::strdup(val);
}

void
XSet::set_next(const std::string& val) noexcept
{
    if (this->next)
        free(this->next);
    this->next = ztd::strdup(val);
}

char*
XSet::get_context() const noexcept
{
    return this->context;
}

void
XSet::set_context(const char* val) noexcept
{
    if (this->context)
        free(this->context);
    this->context = ztd::strdup(val);
}

void
XSet::set_context(const std::string& val) noexcept
{
    if (this->context)
        free(this->context);
    this->context = ztd::strdup(val);
}

bool
XSet::is_tool(XSetTool val) noexcept
{
    return (this->tool == val);
}

bool
XSet::is_tool(const std::vector<XSetTool>& val) noexcept
{
    for (XSetTool v: val)
    {
        if (this->tool == v)
            return true;
    }
    return false;
}

XSetTool
XSet::get_tool() const noexcept
{
    return this->tool;
}

void
XSet::set_tool(XSetTool val) noexcept
{
    this->tool = val;
}

bool
XSet::get_lock() const noexcept
{
    return this->lock;
}

void
XSet::set_lock(bool bval) noexcept
{
    this->lock = bval;
}

char*
XSet::get_prev() const noexcept
{
    return this->prev;
}

void
XSet::set_prev(const char* val) noexcept
{
    if (this->prev)
        free(this->prev);
    this->prev = ztd::strdup(val);
}

void
XSet::set_prev(const std::string& val) noexcept
{
    if (this->prev)
        free(this->prev);
    this->prev = ztd::strdup(val);
}

char*
XSet::get_parent() const noexcept
{
    return this->parent;
}

void
XSet::set_parent(const char* val) noexcept
{
    if (this->parent)
        free(this->parent);
    this->parent = ztd::strdup(val);
}

void
XSet::set_parent(const std::string& val) noexcept
{
    if (this->parent)
        free(this->parent);
    this->parent = ztd::strdup(val);
}

char*
XSet::get_child() const noexcept
{
    return this->child;
}

void
XSet::set_child(const char* val) noexcept
{
    if (this->child)
        free(this->child);
    this->child = ztd::strdup(val);
}

void
XSet::set_child(const std::string& val) noexcept
{
    if (this->child)
        free(this->child);
    this->child = ztd::strdup(val);
}

char*
XSet::get_line() const noexcept
{
    return this->line;
}

void
XSet::set_line(const char* val) noexcept
{
    if (this->line)
        free(this->line);
    this->line = ztd::strdup(val);
}

void
XSet::set_line(const std::string& val) noexcept
{
    if (this->line)
        free(this->line);
    this->line = ztd::strdup(val);
}

bool
XSet::get_task() const noexcept
{
    return this->task;
}

void
XSet::set_task(bool bval) noexcept
{
    this->task = bval;
}

bool
XSet::get_task_pop() const noexcept
{
    return this->task_pop;
}

void
XSet::set_task_pop(bool bval) noexcept
{
    this->task_pop = bval;
}

bool
XSet::get_task_err() const noexcept
{
    return this->task_err;
}

void
XSet::set_task_err(bool bval) noexcept
{
    this->task_err = bval;
}

bool
XSet::get_task_out() const noexcept
{
    return this->task_out;
}

void
XSet::set_task_out(bool bval) noexcept
{
    this->task_out = bval;
}

bool
XSet::get_in_terminal() const noexcept
{
    return this->in_terminal;
}

void
XSet::set_in_terminal(bool bval) noexcept
{
    this->in_terminal = bval;
}

bool
XSet::get_keep_terminal() const noexcept
{
    return this->keep_terminal;
}

void
XSet::set_keep_terminal(bool bval) noexcept
{
    this->keep_terminal = bval;
}

bool
XSet::get_scroll_lock() const noexcept
{
    return this->scroll_lock;
}

void
XSet::set_scroll_lock(bool bval) noexcept
{
    this->scroll_lock = bval;
}

char
XSet::get_opener() const noexcept
{
    return this->opener;
}

void
XSet::set_opener(char val) noexcept
{
    this->opener = val;
}

bool
XSet::get_plugin() const noexcept
{
    return this->plugin;
}

void
XSet::set_plugin(bool bval) noexcept
{
    this->plugin = bval;
}

bool
XSet::get_plugin_top() const noexcept
{
    return this->plugin_top;
}

void
XSet::set_plugin_top(bool bval) noexcept
{
    this->plugin_top = bval;
}

char*
XSet::get_plug_name() const noexcept
{
    return this->plug_name;
}

void
XSet::set_plug_name(const char* val) noexcept
{
    if (this->plug_name)
        free(this->plug_name);
    this->plug_name = ztd::strdup(val);
}

void
XSet::set_plug_name(const std::string& val) noexcept
{
    if (this->plug_name)
        free(this->plug_name);
    this->plug_name = ztd::strdup(val);
}

char*
XSet::get_plug_dir() const noexcept
{
    return this->plug_dir;
}

void
XSet::set_plug_dir(const char* val) noexcept
{
    if (this->plug_dir)
        free(this->plug_dir);
    this->plug_dir = ztd::strdup(val);
}

void
XSet::set_plug_dir(const std::string& val) noexcept
{
    if (this->plug_dir)
        free(this->plug_dir);
    this->plug_dir = ztd::strdup(val);
}

//////////////////////

xset_t
xset_new(const std::string& name, XSetName xset_name) noexcept
{
    xset_t set = new XSet(name, xset_name);

    return set;
}

xset_t
xset_get(const std::string& name) noexcept
{
#ifdef XSET_MAP_TEST
    if (!is_in_xset_map_test(name))
    {
        if (!ztd::startswith(name, "cstm_"))
        {
            // std::cout << name << std::endl;
            LOG_INFO("set name not found in XSetName: {}", name);
        }
    }
#endif

    for (xset_t set: xsets)
    { // check for existing xset
        if (ztd::same(name, set->name))
            return set;
    }

    xset_t set = xset_new(name, xset_get_xsetname_from_name(name));
    xsets.push_back(set);
    return set;
}

xset_t
xset_get(XSetName name) noexcept
{
    for (xset_t set: xsets)
    { // check for existing xset
        if (name == set->xset_name)
            return set;
    }

    xset_t set = xset_new(xset_get_name_from_xsetname(name), name);
    xsets.push_back(set);
    return set;
}

xset_t
xset_is(XSetName name) noexcept
{
    for (xset_t set: xsets)
    { // check for existing xset
        if (name == set->xset_name)
            return set;
    }
    return nullptr;
}

xset_t
xset_is(const std::string& name) noexcept
{
    for (xset_t set: xsets)
    { // check for existing xset
        if (ztd::same(name, set->name))
            return set;
    }
    return nullptr;
}

/////////////////

xset_t
xset_set_var(xset_t set, XSetVar var, const std::string& value) noexcept
{
    if (!set)
        return nullptr;

    switch (var)
    {
        case XSetVar::S:
            set->set_s(value);
            break;
        case XSetVar::B:
            if (ztd::same(value, "1"))
                set->set_b(XSetB::XSET_B_TRUE);
            else
                set->set_b(XSetB::XSET_B_FALSE);
            break;
        case XSetVar::X:
            set->set_x(value);
            break;
        case XSetVar::Y:
            set->set_y(value);
            break;
        case XSetVar::Z:
            set->set_z(value);
            break;
        case XSetVar::KEY:
            set->key = std::stoi(value);
            break;
        case XSetVar::KEYMOD:
            set->keymod = std::stoi(value);
            break;
        case XSetVar::STYLE:
            set->set_menu_style(XSetMenu(std::stoi(value)));
            break;
        case XSetVar::DESC:
            set->set_desc(value);
            break;
        case XSetVar::TITLE:
            set->set_title(value);
            break;
        case XSetVar::MENU_LABEL:
            // lbl is only used >= 0.9.0 for changed lock default menu_label
            set->set_menu_label(value);
            if (set->lock)
                // indicate that menu label is not default and should be saved
                set->set_in_terminal(true);
            break;
        case XSetVar::ICN:
            // icn is only used >= 0.9.0 for changed lock default icon
            set->set_icon(value);
            if (set->lock)
                // indicate that icon is not default and should be saved
                set->set_keep_terminal(true);
            break;
        case XSetVar::MENU_LABEL_CUSTOM:
            // pre-0.9.0 menu_label or >= 0.9.0 custom item label
            // only save if custom or not default label
            if (!set->lock || !ztd::same(set->menu_label, value))
            {
                set->set_menu_label(value);
                if (set->lock)
                    // indicate that menu label is not default and should be saved
                    set->set_in_terminal(true);
            }
            break;
        case XSetVar::ICON:
            // pre-0.9.0 icon or >= 0.9.0 custom item icon
            // only save if custom or not default icon
            // also check that stock name does not match
            break;
        case XSetVar::SHARED_KEY:
            set->set_shared_key(value);
            break;
        case XSetVar::NEXT:
            set->set_next(value);
            break;
        case XSetVar::PREV:
            set->set_prev(value);
            break;
        case XSetVar::PARENT:
            set->set_parent(value);
            break;
        case XSetVar::CHILD:
            set->set_child(value);
            break;
        case XSetVar::CONTEXT:
            set->set_context(value);
            break;
        case XSetVar::LINE:
            set->set_line(value);
            break;
        case XSetVar::TOOL:
            set->set_tool(XSetTool(std::stoi(value)));
            break;
        case XSetVar::TASK:
            if (std::stoi(value) == 1)
                set->set_task(true);
            else
                set->set_task(false);
            break;
        case XSetVar::TASK_POP:
            if (std::stoi(value) == 1)
                set->set_task_pop(true);
            else
                set->set_task_pop(false);
            break;
        case XSetVar::TASK_ERR:
            if (std::stoi(value) == 1)
                set->set_task_err(true);
            else
                set->set_task_err(false);
            break;
        case XSetVar::TASK_OUT:
            if (std::stoi(value) == 1)
                set->set_task_out(true);
            else
                set->set_task_out(false);
            break;
        case XSetVar::RUN_IN_TERMINAL:
            if (std::stoi(value) == 1)
                set->set_in_terminal(true);
            else
                set->set_in_terminal(false);
            break;
        case XSetVar::KEEP_TERMINAL:
            if (std::stoi(value) == 1)
                set->set_keep_terminal(true);
            else
                set->set_keep_terminal(false);
            break;
        case XSetVar::SCROLL_LOCK:
            if (std::stoi(value) == 1)
                set->set_scroll_lock(true);
            else
                set->set_scroll_lock(false);
            break;
        case XSetVar::DISABLE:
            if (std::stoi(value) == 1)
                set->set_disable(true);
            else
                set->set_disable(false);
            break;
        case XSetVar::OPENER:
            set->set_opener(std::stoi(value));
            break;
        default:
            break;
    }

    return set;
}

/**
 * Generic Set
 */

xset_t
xset_set(xset_t set, XSetVar var, const std::string& value) noexcept
{
    if (!set->lock || (var != XSetVar::STYLE && var != XSetVar::DESC && var != XSetVar::TITLE &&
                       var != XSetVar::SHARED_KEY))
        return xset_set_var(set, var, value);
    return set;
}

xset_t
xset_set(XSetName name, XSetVar var, const std::string& value) noexcept
{
    xset_t set = xset_get(name);
    return xset_set(set, var, value);
}

xset_t
xset_set(const std::string& name, XSetVar var, const std::string& value) noexcept
{
    xset_t set = xset_get(name);
    return xset_set(set, var, value);
}

/**
 * S get
 */

char*
xset_get_s(xset_t set) noexcept
{
    return set->get_s();
}

char*
xset_get_s(XSetName name) noexcept
{
    xset_t set = xset_get(name);
    return set->get_s();
}

char*
xset_get_s(const std::string& name) noexcept
{
    xset_t set = xset_get(name);
    return set->get_s();
}

char*
xset_get_s_panel(panel_t panel, const std::string& name) noexcept
{
    // TODO
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    return xset_get_s(fullname);
}

char*
xset_get_s_panel(panel_t panel, XSetPanel name) noexcept
{
    xset_t set = xset_get(xset_get_xsetname_from_panel(panel, name));
    return set->get_s();
}

/**
 * X get
 */

char*
xset_get_x(xset_t set) noexcept
{
    if (set)
        return set->get_x();
    return nullptr;
}

char*
xset_get_x(XSetName name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_x(set);
}

char*
xset_get_x(const std::string& name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_x(set);
}

/**
 * Y get
 */

char*
xset_get_y(xset_t set) noexcept
{
    if (set)
        return set->get_y();
    return nullptr;
}

char*
xset_get_y(XSetName name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_y(set);
}

char*
xset_get_y(const std::string& name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_y(set);
}

/**
 * Z get
 */

char*
xset_get_z(xset_t set) noexcept
{
    if (set)
        return set->get_z();
    return nullptr;
}

char*
xset_get_z(XSetName name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_z(set);
}

char*
xset_get_z(const std::string& name) noexcept
{
    xset_t set = xset_get(name);

    return xset_get_z(set);
}

/**
 * B get
 */

bool
xset_get_b(xset_t set) noexcept
{
    return set->get_b();
}

bool
xset_get_b(XSetName name) noexcept
{
    xset_t set = xset_get(name);
    return set->get_b();
}

bool
xset_get_b(const std::string& name) noexcept
{
    xset_t set = xset_get(name);
    return set->get_b();
}

bool
xset_get_b_set(xset_t set) noexcept
{
    return (set->b == XSetB::XSET_B_TRUE);
}

bool
xset_get_b_panel(panel_t panel, const std::string& name) noexcept
{
    xset_t set = xset_get_panel(panel, name);
    return set->get_b();
}

bool
xset_get_b_panel(panel_t panel, XSetPanel name) noexcept
{
    xset_t set = xset_get(xset_get_xsetname_from_panel(panel, name));
    return set->get_b();
}

bool
xset_get_b_panel_mode(panel_t panel, const std::string& name, MainWindowPanel mode) noexcept
{
    xset_t set = xset_get_panel_mode(panel, name, mode);
    return set->get_b();
}

bool
xset_get_b_panel_mode(panel_t panel, XSetPanel name, MainWindowPanel mode) noexcept
{
    xset_t set = xset_get(xset_get_xsetname_from_panel_mode(panel, name, mode));
    return set->get_b();
}

/**
 * B set
 */
xset_t
xset_set_b(xset_t set, bool bval) noexcept
{
    if (bval)
        set->b = XSetB::XSET_B_TRUE;
    else
        set->b = XSetB::XSET_B_FALSE;
    return set;
}

xset_t
xset_set_b(XSetName name, bool bval) noexcept
{
    xset_t set = xset_get(name);

    if (bval)
        set->b = XSetB::XSET_B_TRUE;
    else
        set->b = XSetB::XSET_B_FALSE;
    return set;
}

xset_t
xset_set_b(const std::string& name, bool bval) noexcept
{
    xset_t set = xset_get(name);

    if (bval)
        set->b = XSetB::XSET_B_TRUE;
    else
        set->b = XSetB::XSET_B_FALSE;
    return set;
}

xset_t
xset_set_b_panel(panel_t panel, const std::string& name, bool bval) noexcept
{
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    xset_t set = xset_set_b(fullname, bval);
    return set;
}

xset_t
xset_set_b_panel(panel_t panel, XSetPanel name, bool bval) noexcept
{
    return xset_set_b(xset_get_xsetname_from_panel(panel, name), bval);
}

xset_t
xset_set_b_panel_mode(panel_t panel, const std::string& name, MainWindowPanel mode,
                      bool bval) noexcept
{
    return xset_set_b(xset_get_panel_mode(panel, name, mode), bval);
}

xset_t
xset_set_b_panel_mode(panel_t panel, XSetPanel name, MainWindowPanel mode, bool bval) noexcept
{
    return xset_set_b(xset_get_xsetname_from_panel_mode(panel, name, mode), bval);
}

/**
 * Panel get
 */

xset_t
xset_get_panel(panel_t panel, const std::string& name) noexcept
{
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    xset_t set = xset_get(fullname);
    return set;
}

xset_t
xset_get_panel(panel_t panel, XSetPanel name) noexcept
{
    return xset_get(xset_get_xsetname_from_panel(panel, name));
}

xset_t
xset_get_panel_mode(panel_t panel, const std::string& name, MainWindowPanel mode) noexcept
{
    const std::string fullname =
        fmt::format("panel{}_{}{}", panel, name, xset_get_window_panel_mode(mode));
    xset_t set = xset_get(fullname);
    return set;
}

xset_t
xset_get_panel_mode(panel_t panel, XSetPanel name, MainWindowPanel mode) noexcept
{
    return xset_get(xset_get_xsetname_from_panel_mode(panel, name, mode));
}

/**
 * Generic Int get
 */

int
xset_get_int_set(xset_t set, XSetVar var) noexcept
{
    if (!set)
        return -1;

    const char* varstring = nullptr;
    switch (var)
    {
        case XSetVar::S:
            varstring = set->s;
            break;
        case XSetVar::X:
            varstring = set->x;
            break;
        case XSetVar::Y:
            varstring = set->y;
            break;
        case XSetVar::Z:
            varstring = set->z;
            break;
        case XSetVar::KEY:
            return set->key;
        case XSetVar::KEYMOD:
            return set->keymod;
        case XSetVar::B:
        case XSetVar::STYLE:
        case XSetVar::DESC:
        case XSetVar::TITLE:
        case XSetVar::MENU_LABEL:
        case XSetVar::ICN:
        case XSetVar::MENU_LABEL_CUSTOM:
        case XSetVar::ICON:
        case XSetVar::SHARED_KEY:
        case XSetVar::NEXT:
        case XSetVar::PREV:
        case XSetVar::PARENT:
        case XSetVar::CHILD:
        case XSetVar::CONTEXT:
        case XSetVar::LINE:
        case XSetVar::TOOL:
        case XSetVar::TASK:
        case XSetVar::TASK_POP:
        case XSetVar::TASK_ERR:
        case XSetVar::TASK_OUT:
        case XSetVar::RUN_IN_TERMINAL:
        case XSetVar::KEEP_TERMINAL:
        case XSetVar::SCROLL_LOCK:
        case XSetVar::DISABLE:
        case XSetVar::OPENER:
        default:
            return -1;
    }

    if (!varstring)
        return 0;
    return std::stol(varstring);
}

int
xset_get_int(XSetName name, XSetVar var) noexcept
{
    xset_t set = xset_get(name);
    return xset_get_int_set(set, var);
}

int
xset_get_int(const std::string& name, XSetVar var) noexcept
{
    xset_t set = xset_get(name);
    return xset_get_int_set(set, var);
}

int
xset_get_int_panel(panel_t panel, const std::string& name, XSetVar var) noexcept
{
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    return xset_get_int(fullname, var);
}

int
xset_get_int_panel(panel_t panel, XSetPanel name, XSetVar var) noexcept
{
    return xset_get_int(xset_get_xsetname_from_panel(panel, name), var);
}

/**
 * Panel Set Generic
 */

xset_t
xset_set_panel(panel_t panel, const std::string& name, XSetVar var,
               const std::string& value) noexcept
{
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    xset_t set = xset_set(fullname, var, value);
    return set;
}

xset_t
xset_set_panel(panel_t panel, XSetPanel name, XSetVar var, const std::string& value) noexcept
{
    return xset_set(xset_get_xsetname_from_panel(panel, name), var, value);
}

/**
 * CB set
 */

xset_t
xset_set_cb(XSetName name, GFunc cb_func, void* cb_data) noexcept
{
    xset_t set = xset_get(name);
    set->cb_func = cb_func;
    set->cb_data = cb_data;
    return set;
}

xset_t
xset_set_cb(const std::string& name, GFunc cb_func, void* cb_data) noexcept
{
    xset_t set = xset_get(name);
    set->cb_func = cb_func;
    set->cb_data = cb_data;
    return set;
}

xset_t
xset_set_cb_panel(panel_t panel, const std::string& name, GFunc cb_func, void* cb_data) noexcept
{
    const std::string fullname = fmt::format("panel{}_{}", panel, name);
    xset_t set = xset_set_cb(fullname, cb_func, cb_data);
    return set;
}

xset_t
xset_set_cb_panel(panel_t panel, XSetPanel name, GFunc cb_func, void* cb_data) noexcept
{
    return xset_set_cb(xset_get_xsetname_from_panel(panel, name), cb_func, cb_data);
}

xset_t
xset_set_ob1_int(xset_t set, const char* ob1, int ob1_int) noexcept
{
    if (set->ob1)
        free(set->ob1);
    set->ob1 = ztd::strdup(ob1);
    set->ob1_data = GINT_TO_POINTER(ob1_int);
    return set;
}

xset_t
xset_set_ob1(xset_t set, const char* ob1, void* ob1_data) noexcept
{
    if (set->ob1)
        free(set->ob1);
    set->ob1 = ztd::strdup(ob1);
    set->ob1_data = ob1_data;
    return set;
}

xset_t
xset_set_ob2(xset_t set, const char* ob2, void* ob2_data) noexcept
{
    if (set->ob2)
        free(set->ob2);
    set->ob2 = ztd::strdup(ob2);
    set->ob2_data = ob2_data;
    return set;
}
