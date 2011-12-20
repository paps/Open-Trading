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

#include "Report.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"
#include "Conf.hpp"

#define CLASS "[Backtester/Report] "

namespace Backtester
{
    Report::Report(Logger const& logger) :
        _logger(logger), _params(logger), _failed(false), _score(0)
    {
    }

    void Report::SetScore(float score)
    {
        this->_score = score;
    }

    float Report::GetScore() const
    {
        return this->_score;
    }

    void Report::CopyParamsFrom(StratParamsMap const& params)
    {
        this->_params.GetDataFrom(params);
    }

    void Report::CopyDataFrom(Report const& report)
    {
        this->_params.GetDataFrom(report.GetParams());
        this->_failed = report.HasFailed();
        this->_trades = report.GetTrades();
    }

    StratParamsMap const& Report::GetParams() const
    {
        return this->_params;
    }

    void Report::DumpParams(bool oneLine /* = false */) const
    {
        this->_params.Dump(oneLine);
    }

    void Report::SetFailed()
    {
        this->_failed = true;
    }

    bool Report::HasFailed() const
    {
        return this->_failed;
    }

    void Report::AddTrade(Trade const& trade)
    {
        this->_trades.push_back(trade);
    }

    std::list<Report::Trade> const& Report::GetTrades() const
    {
        return this->_trades;
    }

    void Report::ShowTradeDetails(Conf const& conf)
    {
        if (!this->_trades.size())
        {
            this->_logger.Log(CLASS "No trades to show.", ::Logger::Warning);
            return;
        }
        this->_logger.Log(CLASS "=== Begin trade report ===");
        unsigned int i = 0;
        std::list<Trade>::const_iterator it = this->_trades.begin();
        std::list<Trade>::const_iterator itEnd = this->_trades.end();
        for (; it != itEnd; ++it)
        {
            ++i;
            this->_logger.Log(CLASS " - Trade " + Tools::ToString(i) + " "
                    + (it->type == Core::Controller::StatusBuy ? " (buy)" : "(sell)") + ": "
                    + "open " + Tools::ToString(it->open, conf.digits) + ", "
                    + "close " + Tools::ToString(it->close, conf.digits) + ", "
                    + "lots " + Tools::ToString(it->lots, 2) + ", "
                    + "SL (at finish) " + Tools::ToString(it->sl, conf.digits) + ", "
                    + "TP (at finish) " + Tools::ToString(it->tp, conf.digits) + ", "
                    + "profit " + conf.baseCurrency + " " + Tools::ToString(it->baseCurrencyProfit, 2) + ", "
                    + "profit " + conf.counterCurrency + " " + Tools::ToString(it->counterCurrencyProfit, 2) + ".");
        }
        this->_logger.Log(CLASS "=== End trade report ===");
    }
}
