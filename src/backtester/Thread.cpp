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

#include "Thread.hpp"
#include "TickGenerator.hpp"
#include "Task.hpp"
#include "StratParamsMap.hpp"
#include "Report.hpp"
#include "ReportManager.hpp"
#include "Backtester.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/Thread] "

namespace Backtester
{
    Thread::Thread(unsigned int id, Conf conf, Core::History& clonableHistory, Backtester& backtester) :
        _id(id), _logger(id), _conf(conf), _history(_logger), _running(false), _thread(0), _backtester(backtester)
    {
        this->_history.CopyDataFrom(clonableHistory);
    }

    Thread::~Thread()
    {
        if (this->_thread != 0)
        {
            this->_logger.Log(CLASS "Join called in destructor.", ::Logger::Warning);
            this->Join();
            delete this->_thread;
        }
    }

    unsigned int Thread::GetId() const
    {
        return this->_id;
    }

    void Thread::Run()
    {
        if (this->_running)
        {
            this->_logger.Log(CLASS "Could not run: thread is already running.", ::Logger::Warning);
            return;
        }
        this->_running = true;
        this->_thread = new boost::thread(boost::bind(&Thread::_Run, this));
    }

    void Thread::Join()
    {
        if (!this->_running)
        {
            this->_logger.Log(CLASS "Could not join: thread is not running.", ::Logger::Warning);
            return;
        }
        this->_thread->join();
        delete this->_thread;
        this->_thread = 0;
    }

    void Thread::_Run()
    {
        StratParamsMap params(this->_logger);
        while (this->_backtester.GetNewParamsFromThread(params))
        {
            Report report(this->_logger);
            this->_Test(params, report);
            this->_backtester.SubmitReportFromThread(report);
        }
    }

    void Thread::_Test(StratParamsMap& stratParams, Report& report)
    {
        this->_logger.Log(CLASS "=== Begin test for generated parameters " + Tools::ToString(stratParams.GetId()) + " ===");
        TickGenerator tickGenerator(this->_history, this->_logger, this->_conf);
        report.CopyParamsFrom(stratParams);
        Task test(tickGenerator, this->_logger, this->_conf, stratParams, report);
        if (test.Run())
            this->_logger.Log(CLASS "=== Test end (success) for generated parameters " + Tools::ToString(stratParams.GetId()) + " ===");
        else
        {
            this->_logger.Log(CLASS "=== Test end (failure) for generated parameters " + Tools::ToString(stratParams.GetId()) + " ===", ::Logger::Error);
            report.SetFailed();
        }
    }
}
