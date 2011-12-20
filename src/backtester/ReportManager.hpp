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

#ifndef __BACKTESTER_REPORTMANAGER__
#define __BACKTESTER_REPORTMANAGER__

#include <boost/noncopyable.hpp>
#include <list>
#include <string>
#include "Logger.hpp"

namespace Backtester
{
    class Report;
    class Conf;
    class ResultRanking;

    class ReportManager :
        private boost::noncopyable
    {
        public:
            explicit ReportManager(Logger const& logger, Conf const& conf);
            ~ReportManager();
            void AddReport(Report const& report);
            void Reset();
            void Run();
            void Log(std::string const& msg, ::Logger::MessageType type = ::Logger::Info) const;
            void ShowTradeDetails();
        private:
            void _ShowReport(Report& report) const;
            ResultRanking* _ResultRankingFactory(std::string const& name) const;
            Logger const& _logger;
            std::list<Report*> _reports;
            std::list<Report*> _failedReports;
            Conf const& _conf;
            ResultRanking* _resultRanking;
    };
}

#endif
