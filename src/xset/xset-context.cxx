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

#include <memory>

#include "xset/xset-context.hxx"

xset_context_t xset_context = nullptr;

XSetContext::XSetContext()
{
    // LOG_INFO("XSetContext Constructor");
}

XSetContext::~XSetContext()
{
    // LOG_INFO("XSetContext Destructor");
}

xset_context_t
xset_context_new()
{
    xset_context = std::make_shared<XSetContext>();
    return xset_context;
}