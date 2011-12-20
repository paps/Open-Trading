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

#ifndef __CONF_CONF__
#define __CONF_CONF__

#include <boost/noncopyable.hpp>
#include "lua/LuaContext.hpp"

namespace Conf
{
    class Conf :
        private boost::noncopyable
    {
        private:
            template <typename T>
                struct AntiDefaultTemplateParameter
                {
                    typedef T Type;
                };

        public:
            explicit Conf(std::string const& path);
            bool Error() const;
            std::string const& GetPath() const;
            bool HasValue(std::string const& name) const;
            std::string const& GetLastError() const;

            template <typename T>
                T Read(std::string const& name, typename AntiDefaultTemplateParameter<T>::Type defaultValue, bool& defaultValueUsed)
                {
                    defaultValueUsed = false;
                    T ret;
                    try
                    {
                        ret = luabind::object_cast<T>(luabind::globals(this->_ctx.Context())[name]);
                    }
                    catch (std::exception&)
                    {
                        defaultValueUsed = true;
                        ret = defaultValue;
                    }
                    return ret;
                }

            template <typename T>
                T Read(std::string const& name, typename AntiDefaultTemplateParameter<T>::Type defaultValue)
                {
                    bool dummy;
                    return this->Read<T>(name, defaultValue, dummy);
                }

        private:
            Lua::LuaContext _ctx;
            bool _error;
            std::string _path;
    };
}

#endif
