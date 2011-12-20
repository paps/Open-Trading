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

#include "StratParamsConf.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/StratParamsConf] "

namespace Core
{
    StratParamsConf::StratParamsConf(std::string const& path, Logger::Logger const& logger) :
        StratParams(logger), _conf(path)
    {
        if (this->_conf.Error())
            this->Log(CLASS "Failed to load parameters file: \"" + this->_conf.GetLastError() + "\".", Logger::Warning);
        else
            this->Log(CLASS "Parameters file \"" + path + "\" loaded successfully.");
    }

    float StratParamsConf::GetFloat(std::string const& name, float defaultValue)
    {
        if (this->_conf.HasValue(name))
        {
            bool defaultValueUsed;
            float ret = this->_conf.Read<float>(name, defaultValue, defaultValueUsed);
            if (defaultValueUsed)
                this->Log(CLASS "Failed to get float parameter \"" + name + "\", using default value: " + Tools::ToString(defaultValue, 2) + ".", Logger::Warning);
            return ret;
        }
        this->Log(CLASS "Float parameter \"" + name + "\" not found, using default value: " + Tools::ToString(defaultValue, 2) + ".", Logger::Warning);
        return defaultValue;
    }

    std::string StratParamsConf::GetString(std::string const& name, std::string const& defaultValue)
    {
        if (this->_conf.HasValue(name))
        {
            bool defaultValueUsed;
            std::string ret = this->_conf.Read<std::string>(name, defaultValue, defaultValueUsed);
            if (defaultValueUsed)
                this->Log(CLASS "Failed to get string parameter \"" + name + "\", using default value: \"" + defaultValue + "\".", Logger::Warning);
            return ret;
        }
        this->Log(CLASS "String parameter \"" + name + "\" not found, using default value: \"" + defaultValue + "\".", Logger::Warning);
        return defaultValue;
    }

    Conf::Conf const& StratParamsConf::GetConf() const
    {
        return this->_conf;
    }
}
