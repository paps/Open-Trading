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

#include "StratParamsMap.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/StratParamsMap] "

namespace Backtester
{
    StratParamsMap::StratParamsMap(Logger::Logger const& logger) :
        StratParams(logger), _id(0)
    {
    }

    void StratParamsMap::GetDataFrom(StratParamsMap const& params)
    {
        this->_floatValues = params.GetFloatMap();
        this->_stringValues = params.GetStringMap();
        this->_id = params.GetId();
    }

    float StratParamsMap::GetFloat(std::string const& name, float defaultValue)
    {
        std::map<std::string, float>::const_iterator it = this->_floatValues.find(name);
        if (it == this->_floatValues.end())
        {
            this->Log(CLASS "Float parameter \"" + name + "\" not found, using default value: " + Tools::ToString(defaultValue, 2) + ".", ::Logger::Warning);
            return defaultValue;
        }
        return it->second;
    }

    std::string StratParamsMap::GetString(std::string const& name, std::string const& defaultValue)
    {
        std::map<std::string, std::string>::const_iterator it = this->_stringValues.find(name);
        if (it == this->_stringValues.end())
        {
            this->Log(CLASS "String parameter \"" + name + "\" not found, using default value: \"" + defaultValue + "\".", ::Logger::Warning);
            return defaultValue;
        }
        return it->second;
    }

    void StratParamsMap::SetFloat(std::string const& name, float value)
    {
        this->_floatValues[name] = value;
    }

    void StratParamsMap::SetString(std::string const& name, std::string value)
    {
        this->_stringValues[name] = value;
    }

    void StratParamsMap::Reset()
    {
        this->_id = 0;
        this->_floatValues.clear();
        this->_stringValues.clear();
    }

    void StratParamsMap::SetId(unsigned int id)
    {
        this->_id = id;
    }

    unsigned int StratParamsMap::GetId() const
    {
        return this->_id;
    }

    std::map<std::string, float> const& StratParamsMap::GetFloatMap() const
    {
        return this->_floatValues;
    }

    std::map<std::string, std::string> const& StratParamsMap::GetStringMap() const
    {
        return this->_stringValues;
    }

    std::string StratParamsMap::GetFloatParamsString() const
    {
        std::string ret = "Parameters " + Tools::ToString(this->_id) + ":";
        std::map<std::string, float>::const_iterator it = this->_floatValues.begin();
        std::map<std::string, float>::const_iterator itEnd = this->_floatValues.end();
        for (; it != itEnd; ++it)
            ret += " " + it->first + " " + Tools::ToString(it->second, 2);
        ret += ".";
        return ret;
    }

    void StratParamsMap::Dump(bool oneLine /* = false */) const
    {
        if (oneLine)
        {
            std::string log;
            {
                log += "Floats:";
                std::map<std::string, float>::const_iterator it = this->_floatValues.begin();
                std::map<std::string, float>::const_iterator itEnd = this->_floatValues.end();
                for (; it != itEnd; ++it)
                    log += " \"" + it->first + "\": " + Tools::ToString(it->second, 2);
            }
            {
                log += " Strings:";
                std::map<std::string, std::string>::const_iterator it = this->_stringValues.begin();
                std::map<std::string, std::string>::const_iterator itEnd = this->_stringValues.end();
                for (; it != itEnd; ++it)
                    log += " \"" + it->first + "\": \"" + it->second + "\"";
            }
            this->Log(CLASS + log);
        }
        else
        {
            this->Log(CLASS "Parameters dump:");
            this->Log(CLASS "  Id: " + Tools::ToString(this->_id) + ".");
            {
                this->Log(CLASS "  Float values:");
                std::map<std::string, float>::const_iterator it = this->_floatValues.begin();
                std::map<std::string, float>::const_iterator itEnd = this->_floatValues.end();
                for (; it != itEnd; ++it)
                    this->Log(CLASS "    \"" + it->first + "\": " + Tools::ToString(it->second, 2) + ".");
            }
            {
                this->Log(CLASS "  String values:");
                std::map<std::string, std::string>::const_iterator it = this->_stringValues.begin();
                std::map<std::string, std::string>::const_iterator itEnd = this->_stringValues.end();
                for (; it != itEnd; ++it)
                    this->Log(CLASS "    \"" + it->first + "\": \"" + it->second + "\".");
            }
        }
    }
}
