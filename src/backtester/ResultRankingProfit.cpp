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

#include "ResultRankingProfit.hpp"
#include "Report.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/ResultRankingProfit] "

namespace Backtester
{
    ResultRankingProfit::ResultRankingProfit(Logger const& logger, Conf const& conf) :
        ResultRanking("profit", logger, conf)
    {
    }

    float ResultRankingProfit::Rank(Report const& report) const
    {
        float profit = 0;
        std::list<Report::Trade>::const_iterator it = report.GetTrades().begin();
        std::list<Report::Trade>::const_iterator itEnd = report.GetTrades().end();
        for (; it != itEnd; ++it)
            profit += it->counterCurrencyProfit;
        //this->_logger.Log(CLASS + report.GetParams().GetFloatParamsString() + " score " + Tools::ToString(profit));
        return profit;
    }
}
