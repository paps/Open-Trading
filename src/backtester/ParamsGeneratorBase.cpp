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

#include "ParamsGeneratorBase.hpp"
#include "StratParamsMap.hpp"

namespace Backtester
{
    ParamsGeneratorBase::ParamsGeneratorBase(Logger const& logger, Conf& conf) :
        ParamsGenerator("base", logger, conf), _done(false)
    {
    }

    bool ParamsGeneratorBase::GenerateNextParams(StratParamsMap& params)
    {
        if (this->_done)
            return false;
        {
            std::list<FloatParam>::const_iterator it = this->_floatParams.begin();
            std::list<FloatParam>::const_iterator itEnd = this->_floatParams.end();
            for (; it != itEnd; ++it)
                params.SetFloat(it->name, it->value);
        }
        {
            std::list<StringParam>::const_iterator it = this->_stringParams.begin();
            std::list<StringParam>::const_iterator itEnd = this->_stringParams.end();
            for (; it != itEnd; ++it)
                params.SetString(it->name, it->value);
        }
        this->_done = true;
        return true;
    }

    bool ParamsGeneratorBase::_AddFloatParam(std::string const& name, float start, float, unsigned int)
    {
        FloatParam p;
        p.name = name;
        p.value = start;
        this->_floatParams.push_back(p);
        return true;
    }

    bool ParamsGeneratorBase::_AddStringParam(std::string const& name, std::string const& value)
    {
        StringParam p;
        p.name = name;
        p.value = value;
        this->_stringParams.push_back(p);
        return true;
    }

    unsigned int ParamsGeneratorBase::GetNbTotalTasks() const
    {
        return 1;
    }
}
