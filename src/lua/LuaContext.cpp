// The Open Trading Project - open-trading.org
//
// Copyright (c) 2011 Martin Tapia - martin.tapia@open-trading.org
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "LuaContext.hpp"

namespace Lua
{
    LuaContext::LuaContext(ContextType type) :
        _type(type)
    {
        this->_Init();
    }

    LuaContext::~LuaContext()
    {
        this->_Destroy();
    }

    void LuaContext::_Init()
    {
        this->_luaState = lua_open();
        lua_pushcfunction(this->_luaState, luaopen_base);
        lua_pushstring(this->_luaState, "");
        lua_call(this->_luaState, 1, 0);
        if (this->_type == Scripting)
        {
            lua_pushcfunction(this->_luaState, luaopen_string);
            lua_pushstring(this->_luaState, LUA_STRLIBNAME);
            lua_call(this->_luaState, 1, 0);
            lua_pushcfunction(this->_luaState, luaopen_table);
            lua_pushstring(this->_luaState, LUA_TABLIBNAME);
            lua_call(this->_luaState, 1, 0);
            lua_pushcfunction(this->_luaState, luaopen_math);
            lua_pushstring(this->_luaState, LUA_MATHLIBNAME);
            lua_call(this->_luaState, 1, 0);
            lua_pushcfunction(this->_luaState, luaopen_debug);
            lua_pushstring(this->_luaState, LUA_DBLIBNAME);
            lua_call(this->_luaState, 1, 0);
        }
        luabind::open(this->_luaState);
    }

    void LuaContext::_Destroy()
    {
        lua_close(this->_luaState);
        this->_luaState = 0;
    }

    bool LuaContext::LoadFile(std::string const& path)
    {
        bool ret = ((luaL_loadfile(this->_luaState, path.c_str()) || lua_pcall(this->_luaState, 0, LUA_MULTRET, 0)) == 0);
        if (!ret)
        {
            try
            {
                this->_lastError = luabind::object_cast<std::string>(luabind::object(luabind::from_stack(this->_luaState, -1)));
            }
            catch (std::exception&)
            {
                this->_lastError = "could not get error string";
            }
        }
        return ret;
    }

    std::string const& LuaContext::GetLastError() const
    {
        return this->_lastError;
    }

    void LuaContext::Reset()
    {
        this->_Destroy();
        this->_Init();
    }

    lua_State* LuaContext::Context() const
    {
        return this->_luaState;
    }
}
