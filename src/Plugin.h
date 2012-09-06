/*
    OpenSR - opensource multi-genre game based upon "Space Rangers 2: Dominators"
    Copyright (C) 2012 Kosyak <ObKo@mail.ru>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RANGERS_PLUGIN_H
#define RANGERS_PLUGIN_H

#include "config.h"
#include <string>

struct lua_State;

namespace Rangers
{
class RANGERS_ENGINE_API Plugin
{
public:
    Plugin(const std::wstring& path);

    int load();
    bool isLoaded() const;

    void initLua(lua_State *lua);

    ~Plugin();

private:
    bool m_loaded;
    std::wstring m_path;
    int (*m_rangersPluginInit)();
    int (*m_rangersAPIVersion)();
    void (*m_rangersPluginInitLua)(lua_State *state);
    void (*m_rangersPluginDeinit)();
#ifdef _WIN32
#else
    void *m_handle;
#endif
};
}

#endif // RANGERS_PLUGIN_H
