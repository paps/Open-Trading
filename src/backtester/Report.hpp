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

#ifndef __BACKTESTER_REPORT__
#define __BACKTESTER_REPORT__

#include <boost/noncopyable.hpp>
#include <list>
#include "StratParamsMap.hpp"
#include "core/Controller.hpp"

namespace Backtester
{
    class Logger;
    class Conf;

    class Report :
        private boost::noncopyable
    {
        public:
            struct Trade
            {
                Core::Controller::Status type;
                float open;
                float close;
                float lots;
                float sl;
                float tp;
                float pips;
                float baseCurrencyProfit;
                float counterCurrencyProfit;
            };
            explicit Report(Logger const& logger);
            void CopyParamsFrom(StratParamsMap const& params);
            void CopyDataFrom(Report const& report);
            StratParamsMap const& GetParams() const;
            void DumpParams(bool oneLine = false) const;
            bool HasFailed() const;
            void SetFailed();
            void AddTrade(Trade const& trade);
            std::list<Trade> const& GetTrades() const;
            void ShowTradeDetails(Conf const& conf);
            void SetScore(float score);
            float GetScore() const;
        private:
            Logger const& _logger;
            StratParamsMap _params;
            bool _failed;
            std::list<Trade> _trades;
            float _score;
    };
}

#endif
