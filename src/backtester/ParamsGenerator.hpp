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

#ifndef __BACKTESTER_PARAMSGENERATOR__
#define __BACKTESTER_PARAMSGENERATOR__

#include <boost/noncopyable.hpp>
#include "lua/LuaContext.hpp"

namespace Backtester
{
    class Logger;
    class Conf;
    class StratParamsMap;
    class Report;

    class ParamsGenerator :
        private boost::noncopyable
    {
        public:
            ParamsGenerator(std::string const& name, Logger const& logger, Conf const& conf);
            virtual ~ParamsGenerator();
            bool ProcessFile(std::string const& file);
            virtual bool Initialize();
            virtual bool GenerateNextParams(StratParamsMap& params) = 0;
            virtual void ReportFeedback(Report const& report);
            virtual unsigned int GetNbTotalTasks() const;
            std::string const& GetName() const;
        protected:
            Logger const& _logger;
            Conf const& _conf;
        private:
            virtual bool _AddFloatParam(std::string const& name, float start, float step, unsigned int iterations) = 0;
            virtual bool _AddStringParam(std::string const& name, std::string const& value) = 0;
            Lua::LuaContext _lua;
            std::string _name;
    };
}

#endif
