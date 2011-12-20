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

#ifndef __BACKTESTER_BACKTESTER__
#define __BACKTESTER_BACKTESTER__

#include <boost/noncopyable.hpp>
#include <thread>
#include "tools/Timer.hpp"

namespace Core
{
    class History;
}

namespace Backtester
{
    class Logger;
    class Conf;
    class StratParamsMap;
    class Report;
    class ReportManager;
    class ParamsGenerator;

    class Backtester :
        private boost::noncopyable
    {
        public:
            explicit Backtester(Logger const& logger, Conf& conf, Core::History& history);
            ~Backtester();
            void Run();
            bool GetNewParamsFromThread(StratParamsMap& params);
            void SubmitReportFromThread(Report const& report);
        private:
            ParamsGenerator* _ParamsGeneratorFactory(std::string const& name) const;
            Logger const& _logger;
            Conf& _conf;
            Core::History& _history;
            std::mutex _mutex;
            ReportManager* _reportManager;
            ParamsGenerator* _paramsGenerator;
            unsigned int _nbFinishedTasks;
            Tools::Timer _timer;
    };
}

#endif
