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

#include "ReportManager.hpp"
#include "Report.hpp"
#include "Conf.hpp"
#include "ResultRankingProfit.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/ReportManager] "

namespace Backtester
{
    namespace
    {
        bool CompareReports(Report* r1, Report* r2)
        {
            return r1->GetScore() < r2->GetScore();
        }
    }

    ReportManager::ReportManager(Logger const& logger, Conf const& conf) :
        _logger(logger), _conf(conf)
    {
        this->_resultRanking = this->_ResultRankingFactory(this->_conf.resultRanking);
        this->Log(CLASS "Using result ranking \"" + this->_resultRanking->GetName() + "\".");
    }

    ReportManager::~ReportManager()
    {
        delete this->_resultRanking;
        this->Reset();
    }

    ResultRanking* ReportManager::_ResultRankingFactory(std::string const& name) const
    {
        if (name == "profit")
            return new ResultRankingProfit(this->_logger, this->_conf);
        else
            this->Log(CLASS "Result ranking \"" + name + "\" not found, using default \"profit\".", ::Logger::Warning);
        return new ResultRankingProfit(this->_logger, this->_conf);
    }

    void ReportManager::Reset()
    {
        {
            std::list<Report*>::iterator it = this->_reports.begin();
            std::list<Report*>::iterator itEnd = this->_reports.end();
            for (; it != itEnd; ++it)
                delete *it;
            this->_reports.clear();
        }
        {
            std::list<Report*>::iterator it = this->_failedReports.begin();
            std::list<Report*>::iterator itEnd = this->_failedReports.end();
            for (; it != itEnd; ++it)
                delete *it;
            this->_failedReports.clear();
        }
    }

    void ReportManager::AddReport(Report const& report)
    {
        Report* r = new Report(this->_logger);
        r->CopyDataFrom(report);
        if (r->HasFailed())
            this->_failedReports.push_back(r);
        else
            this->_reports.push_back(r);
    }

    void ReportManager::ShowTradeDetails()
    {
        if (this->_reports.size() == 1)
            (*this->_reports.begin())->ShowTradeDetails(this->_conf);
        else
            this->Log(CLASS "Could not show trade details: " + Tools::ToString(this->_reports.size()) + " reports stored.", ::Logger::Warning);
    }

    void ReportManager::Run()
    {
        this->Log(CLASS + Tools::ToString(this->_reports.size()) + " successful report" + (this->_reports.size() > 1 ? "s" : "") + " collected.");
        this->Log(CLASS "Reports marked as failed: " + Tools::ToString(this->_failedReports.size()) + ".", this->_failedReports.size() ? ::Logger::Warning : ::Logger::Info);
        {
            std::list<Report*>::iterator it = this->_reports.begin();
            std::list<Report*>::iterator itEnd = this->_reports.end();
            for (; it != itEnd; ++it)
                (*it)->SetScore(this->_resultRanking->Rank(**it));
        }
        this->_reports.sort(CompareReports);
        if (this->_reports.size() > 1)
        {
            this->Log(CLASS "Sorted reports (result ranking \"" + this->_resultRanking->GetName() + "\"):");
            std::list<Report*>::iterator it = this->_reports.begin();
            std::list<Report*>::iterator itEnd = this->_reports.end();
            for (; it != itEnd; ++it)
                this->Log(CLASS + (*it)->GetParams().GetFloatParamsString() + " Score " + Tools::ToString((*it)->GetScore()));
            this->_ShowReport(**this->_reports.rbegin());
        }
        else if (this->_reports.size() == 1)
            this->_ShowReport(**this->_reports.begin());
        else
            this->Log(CLASS "No results to show.", ::Logger::Warning);
    }

    void ReportManager::_ShowReport(Report& report) const
    {
        this->Log(CLASS "=== Begin results ===");
        this->Log(CLASS "Parameters:");
        report.DumpParams();
        this->Log(CLASS "Results:");
        float balance = this->_conf.deposit;
        unsigned int profitTrades = 0;
        unsigned int profitBuyTrades = 0;
        unsigned int profitSellTrades = 0;
        unsigned int lossTrades = 0;
        unsigned int lossBuyTrades = 0;
        unsigned int lossSellTrades = 0;
        std::list<Report::Trade> const& trades = report.GetTrades();
        std::list<Report::Trade>::const_iterator it = trades.begin();
        std::list<Report::Trade>::const_iterator itEnd = trades.end();
        for (; it != itEnd; ++it)
        {
            balance += it->counterCurrencyProfit;
            if (it->counterCurrencyProfit > 0)
            {
                ++profitTrades;
                if (it->type == Core::Controller::StatusBuy)
                    ++profitBuyTrades;
                else
                    ++profitSellTrades;
            }
            else
            {
                ++lossTrades;
                if (it->type == Core::Controller::StatusBuy)
                    ++lossBuyTrades;
                else
                    ++lossSellTrades;
            }
        }
        this->Log(CLASS " - Balance: " + this->_conf.counterCurrency + " " + Tools::ToString(balance, 2) + ".");
        this->Log(CLASS " - Profit: " + this->_conf.counterCurrency + " " + Tools::ToString(balance - this->_conf.deposit, 2) + ".");
        this->Log(CLASS " - Trades:\t\tP\tP%\tL&E\tL&E%");
        this->Log(CLASS "   - All\t" +
                Tools::ToString(profitTrades + lossTrades) + "\t" +
                Tools::ToString(profitTrades) + "\t" +
                Tools::ToString((profitTrades / static_cast<float>(lossTrades + profitTrades)) * 100., 2) + "\t" +
                Tools::ToString(lossTrades) + "\t" +
                Tools::ToString((lossTrades / static_cast<float>(lossTrades + profitTrades)) * 100., 2));
        this->Log(CLASS "   - Buy\t" +
                Tools::ToString(profitBuyTrades + lossBuyTrades) + "\t" +
                Tools::ToString(profitBuyTrades) + "\t" +
                Tools::ToString((profitBuyTrades / static_cast<float>(lossBuyTrades + profitBuyTrades)) * 100., 2) + "\t" +
                Tools::ToString(lossBuyTrades) + "\t" +
                Tools::ToString((lossBuyTrades / static_cast<float>(lossBuyTrades + profitBuyTrades)) * 100., 2));
        this->Log(CLASS "   - Sell\t" +
                Tools::ToString(profitSellTrades + lossSellTrades) + "\t" +
                Tools::ToString(profitSellTrades) + "\t" +
                Tools::ToString((profitSellTrades / static_cast<float>(lossSellTrades + profitSellTrades)) * 100., 2) + "\t" +
                Tools::ToString(lossSellTrades) + "\t" +
                Tools::ToString((lossSellTrades / static_cast<float>(lossSellTrades + profitSellTrades)) * 100., 2));
        this->Log(CLASS "=== End results ===");
    }

    void ReportManager::Log(std::string const& msg, ::Logger::MessageType type /* = ::Logger::Info */) const
    {
        this->_logger.Log(msg, type);
    }
}
