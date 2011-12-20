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

#include "ParamsGenerator.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/ParamsGenerator] "

namespace Backtester
{
    ParamsGenerator::ParamsGenerator(std::string const& name, Logger const& logger, Conf const& conf) :
         _logger(logger), _conf(conf), _lua(Lua::LuaContext::Configuration), _name(name)
    {
    }

    ParamsGenerator::~ParamsGenerator()
    {
    }

    std::string const& ParamsGenerator::GetName() const
    {
        return this->_name;
    }

    bool ParamsGenerator::Initialize()
    {
        return true;
    }

    void ParamsGenerator::ReportFeedback(Report const&)
    {
    }

    unsigned int ParamsGenerator::GetNbTotalTasks() const
    {
        return 0;
    }

    bool ParamsGenerator::ProcessFile(std::string const& file)
    {
        // load file
        if (!this->_lua.LoadFile(file))
        {
            this->_logger.Log(CLASS "Failed to load parameters file \"" + file + "\": \"" + this->_lua.GetLastError() + "\".", ::Logger::Error);
            return false;
        }
        else
            this->_logger.Log(CLASS "Using parameters file \"" + file + "\" for parameters generator \"" + this->_name + "\".");
        // parse file
        try
        {
            // param table
            if (luabind::type(luabind::globals(this->_lua.Context())["param"]) == LUA_TTABLE)
            {
                luabind::iterator it(luabind::globals(this->_lua.Context())["param"]);
                luabind::iterator itEnd;
                for (; it != itEnd; ++it)
                {
                    std::string name = luabind::object_cast<std::string>(it.key());
                    if (luabind::type(*it) != LUA_TTABLE)
                    {
                        this->_logger.Log(CLASS "Invalid parameter \"" + name + "\".", ::Logger::Warning);
                        continue;
                    }
                    float start = luabind::object_cast<float>((*it)["start"]);
                    float step = luabind::object_cast<float>((*it)["step"]);
                    unsigned int iterations = luabind::object_cast<unsigned int>((*it)["iterations"]);
                    if (iterations < 1)
                    {
                        iterations = 1;
                        this->_logger.Log(CLASS "Invalid iterations value for parameter \"" + name + "\", changing to " + Tools::ToString(iterations) + ".", ::Logger::Warning);
                    }
                    if (iterations > 1 && !step)
                    {
                        step = 0.01; // shitty
                        this->_logger.Log(CLASS "Invalid step value for parameter \"" + name + "\", changing to " + Tools::ToString(step, 2) + ".", ::Logger::Warning);
                    }
                    if (!this->_AddFloatParam(name, start, step, iterations))
                    {
                        this->_logger.Log(CLASS "Failed to add float paramter \"" + name + "\".", ::Logger::Error);
                        return false;
                    }
                }
            }
            // string table
            if (luabind::type(luabind::globals(this->_lua.Context())["string"]) == LUA_TTABLE)
            {
                luabind::iterator it(luabind::globals(this->_lua.Context())["string"]);
                luabind::iterator itEnd;
                for (; it != itEnd; ++it)
                {
                    std::string name = luabind::object_cast<std::string>(it.key());
                    if (luabind::type(*it) != LUA_TSTRING)
                    {
                        this->_logger.Log(CLASS "Invalid string \"" + name + "\".", ::Logger::Warning);
                        continue;
                    }
                    std::string value = luabind::object_cast<std::string>(*it);
                    if (!this->_AddStringParam(name, value))
                    {
                        this->_logger.Log(CLASS "Failed to add string paramter \"" + name + "\".", ::Logger::Error);
                        return false;
                    }
                }
            }
            return true;
        }
        catch (std::exception& e)
        {
            this->_logger.Log(CLASS "Exception while parsing parameters file: \"" + std::string(e.what()) + "\".", ::Logger::Error);
            return false;
        }
        return true;
    }
}
