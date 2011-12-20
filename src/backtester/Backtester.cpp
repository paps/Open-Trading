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

#include "Backtester.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"
#include "ParamsGeneratorComplete.hpp"
#include "ParamsGeneratorBase.hpp"
#include "Conf.hpp"
#include "Thread.hpp"
#include "StratParamsMap.hpp"
#include "ReportManager.hpp"
#include "Report.hpp"

#define CLASS "[Backtester/Backtester] "

namespace Backtester
{
    Backtester::Backtester(Logger const& logger, Conf& conf, Core::History& history) :
        _logger(logger), _conf(conf), _history(history), _nbFinishedTasks(0)
    {
        this->_paramsGenerator = this->_ParamsGeneratorFactory(this->_conf.paramsGenerator);
        this->_reportManager = new ReportManager(this->_logger, this->_conf);
    }

    Backtester::~Backtester()
    {
        delete this->_reportManager;
        delete this->_paramsGenerator;
    }

    ParamsGenerator* Backtester::_ParamsGeneratorFactory(std::string const& name) const
    {
        if (this->_conf.optimizationMode)
        {
            if (name == "complete")
                return new ParamsGeneratorComplete(this->_logger, this->_conf);
            else
                this->_logger.Log(CLASS "Paramters generator \"" + name + "\" not found, using default \"base\".", ::Logger::Warning);
        }
        return new ParamsGeneratorBase(this->_logger, this->_conf);
    }

    bool Backtester::GetNewParamsFromThread(StratParamsMap& params)
    {
        params.Reset();
        std::lock_guard<std::mutex> lock(this->_mutex);
        return this->_paramsGenerator->GenerateNextParams(params);
    }

    void Backtester::SubmitReportFromThread(Report const& report)
    {
        std::lock_guard<std::mutex> lock(this->_mutex);
        ++this->_nbFinishedTasks;
        this->_logger.Log(CLASS "Task " + Tools::ToString(this->_nbFinishedTasks) + " report (" + (report.HasFailed() ? "failed" : "success") + ", " +
                Tools::ToString(report.GetTrades().size()) + " trades) with " + report.GetParams().GetFloatParamsString());
        unsigned int totalTasks = this->_paramsGenerator->GetNbTotalTasks();
        if (this->_nbFinishedTasks >= totalTasks)
            this->_logger.Log(CLASS "Task " + Tools::ToString(this->_nbFinishedTasks) + " finished.");
        else
        {
            int tasksLeft = totalTasks - this->_nbFinishedTasks;
            long time = this->_timer.ElapsedMs() / this->_nbFinishedTasks; // time per task
            time *= (tasksLeft > 0 ? tasksLeft : 0); // time for all the left tasks
            time /= 1000; // in seconds
            unsigned int hours = time / 3600;
            unsigned int minutes = (time - hours * 3600) / 60;
            unsigned int seconds = time - hours * 3600 - minutes * 60;
            this->_logger.Log(CLASS "Task " + Tools::ToString(this->_nbFinishedTasks) + "/" +
                    (totalTasks ? Tools::ToString(totalTasks) : "?") + " finished. Estimated time left: " +
                    Tools::ToString(hours) + "h " + Tools::ToString(minutes) + "m " + Tools::ToString(seconds) + "s.");
        }
        this->_reportManager->AddReport(report);
    }

    void Backtester::Run()
    {
        // initialize parameter generator
        if (!this->_paramsGenerator->ProcessFile(this->_conf.strategyParams))
        {
            this->_logger.Log(CLASS "Parameters generator failed to process file \"" + this->_conf.strategyParams + "\".", ::Logger::Error);
            return;
        }
        if (!this->_paramsGenerator->Initialize())
        {
            this->_logger.Log(CLASS "Parameters generator failed to initialize.", ::Logger::Error);
            return;
        }

        // create threads
        std::vector<Thread*> threads;
        for (unsigned int i = 0; i < this->_conf.threads; ++i)
            threads.push_back(new Thread(i + 1, this->_conf, this->_history, *this));

        // log recap
        this->_logger.Log(CLASS + std::string("Optimization mode: ") + (this->_conf.optimizationMode ? "enabled" : "disabled (one thread)") + ".");
        if (this->_conf.optimizationMode)
        {
            if (this->_paramsGenerator->GetNbTotalTasks())
                this->_logger.Log(CLASS "Estimated number of tasks: " + Tools::ToString(this->_paramsGenerator->GetNbTotalTasks()) + " (" +
                        Tools::ToString(this->_conf.threads) + " thread" + (this->_conf.threads > 1 ? "s" : "") + ", ~" +
                        Tools::ToString(static_cast<float>(this->_paramsGenerator->GetNbTotalTasks()) / static_cast<float>(this->_conf.threads), 1) + " tasks per thread).");
            else
                this->_logger.Log(CLASS "Unknown number of tasks.", ::Logger::Warning);
        }

        // ask user confirmation
        if (this->_conf.confirmLaunch)
        {
            this->_logger.Log(CLASS "Proceed [Y/n]? ");
            int c = getchar();
            if (c == 'n' || c == 'N')
            {
                this->_logger.Log(CLASS "Abort.", ::Logger::Error);
                std::vector<Thread*>::iterator it = threads.begin();
                std::vector<Thread*>::iterator itEnd = threads.end();
                for (; it != itEnd; ++it)
                    delete *it;
                return;
            }
        }

        // launch threads
        this->_timer.Reset();
        {
            std::vector<Thread*>::iterator it = threads.begin();
            std::vector<Thread*>::iterator itEnd = threads.end();
            for (; it != itEnd; ++it)
                (*it)->Run();
        }

        // wait for threads
        {
            std::vector<Thread*>::iterator it = threads.begin();
            std::vector<Thread*>::iterator itEnd = threads.end();
            for (; it != itEnd; ++it)
            {
                (*it)->Join();
                this->_logger.Log(CLASS "Thread " + Tools::ToString((*it)->GetId()) + " finished.");
                delete *it;
            }
        }

        // show results
        if (!this->_conf.optimizationMode && this->_conf.showTradeDetails)
            this->_reportManager->ShowTradeDetails();
        this->_reportManager->Run();
    }
}
