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

#include "ParamsGeneratorComplete.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"
#include "StratParamsMap.hpp"
#include "Conf.hpp"

#define CLASS "[Backtester/ParamsGeneratorComplete] "

namespace Backtester
{
    ParamsGeneratorComplete::ParamsGeneratorComplete(Logger const& logger, Conf& conf) :
        ParamsGenerator("complete", logger, conf), _noMoreParams(false), _nextParamId(0), _nbTasks(0)
    {
    }

    bool ParamsGeneratorComplete::Initialize()
    {
        if (this->_floatParams.size())
        {
            this->_nbTasks = 1;
            std::string floatRes = Tools::ToString(this->_floatParams.size()) + " parameter" + (this->_floatParams.size() > 1 ? "s" : "") + ".";
            std::vector<FloatParam>::const_iterator it = this->_floatParams.begin();
            std::vector<FloatParam>::const_iterator itEnd = this->_floatParams.end();
            for (; it != itEnd; ++it)
            {
                floatRes += " \"" + it->name + "\" (" + Tools::ToString(it->start, 2) + ", " + Tools::ToString(it->step, 2) + ", " + Tools::ToString(it->iterations) + ")";
                this->_nbTasks *= it->iterations;
            }
            this->_logger.Log(CLASS + floatRes);
        }
        else
            this->_logger.Log(CLASS "No parameters.");
        if (this->_stringParams.size())
        {
            std::string stringRes = Tools::ToString(this->_stringParams.size()) + " string" + (this->_stringParams.size() > 1 ? "s" : "") + ".";
            std::vector<StringParam>::const_iterator it = this->_stringParams.begin();
            std::vector<StringParam>::const_iterator itEnd = this->_stringParams.end();
            for (; it != itEnd; ++it)
                stringRes += " \"" + it->name + "\" (" + Tools::ToString(it->value.size()) + ")";
            this->_logger.Log(CLASS + stringRes);
        }
        else
            this->_logger.Log(CLASS "No strings.");
        return true;
    }

    unsigned int ParamsGeneratorComplete::GetNbTotalTasks() const
    {
        return this->_nbTasks;
    }

    bool ParamsGeneratorComplete::_AddFloatParam(std::string const& name, float start, float step, unsigned int iterations)
    {
        FloatParam p;
        p.name = name;
        p.start = start;
        p.step = step;
        p.iterations = iterations;
        p.current = p.start;
        this->_floatParams.push_back(p);
        return true;
    }

    bool ParamsGeneratorComplete::_AddStringParam(std::string const& name, std::string const& value)
    {
        StringParam p;
        p.name = name;
        p.value = value;
        this->_stringParams.push_back(p);
        return true;
    }

    bool ParamsGeneratorComplete::GenerateNextParams(StratParamsMap& params)
    {
        // check finished generation
        if (this->_noMoreParams)
            return false;

        // check empty parameters (parsing failure / empty parameters file)
        if (this->_floatParams.empty() || !this->_conf.optimizationMode)
        {
            this->_WriteParams(params); // write strings
            this->_noMoreParams = true;
            return true;
        }

        // return the current parameters
        this->_WriteParams(params);

        // generate the next parameters
        unsigned int pos = this->_floatParams.size() - 1;
        while (true)
        {
            FloatParam& p = this->_floatParams[pos];
            p.current += p.step;
            if (p.iterations <= 1 || p.current >= p.start + p.step * p.iterations)
            {
                if (!pos)
                {
                    this->_noMoreParams = true;
                    break;
                }
                else
                {
                    p.current = p.start;
                    --pos;
                }
            }
            else
                break;
        }
        return true;
    }

    void ParamsGeneratorComplete::_WriteParams(StratParamsMap& params)
    {
        ++this->_nextParamId;
        params.SetId(this->_nextParamId);
        {
            std::vector<FloatParam>::const_iterator it = this->_floatParams.begin();
            std::vector<FloatParam>::const_iterator itEnd = this->_floatParams.end();
            for (; it != itEnd; ++it)
                params.SetFloat(it->name, it->current);
        }
        {
            std::vector<StringParam>::const_iterator it = this->_stringParams.begin();
            std::vector<StringParam>::const_iterator itEnd = this->_stringParams.end();
            for (; it != itEnd; ++it)
                params.SetString(it->name, it->value);
        }
    }
}
