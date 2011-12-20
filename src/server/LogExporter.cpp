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

#include "LogExporter.hpp"
#include "Logger.hpp"

#define CLASS "[Server/LogExporter] "

namespace Server
{
    LogExporter::LogExporter(Logger const& logger) :
        _logger(logger), _running(false), _stopFlag(false)
    {
    }

    LogExporter::~LogExporter()
    {
        if (this->_running)
            this->Stop();
    }

    void LogExporter::Start()
    {
        if (this->_running)
        {
            this->_logger.Log(CLASS "Start called while running.", ::Logger::Error);
            return;
        }
        this->_running = true;
        this->_Start();
        this->_logger.Log(CLASS "Log exporter started.");
    }

    void LogExporter::Stop()
    {
        if (!this->_running)
        {
            this->_logger.Log(CLASS "Stop called while not running.", ::Logger::Error);
            return;
        }
        this->_running = false;
        this->_Stop();
        this->_logger.Log(CLASS "Log exporter stopped.");
    }

    void LogExporter::Log(QString const& /* string */)
    {
        boost::lock_guard<boost::mutex> lock(this->_mutex);
        // TODO buffer stuff
    }

    void LogExporter::_Work(boost::unique_lock<boost::mutex>& lock)
    {
        return; // XXX
        while (true)
        {
            // TODO fetch buffered stuff (if empty, return here)
            lock.unlock();
            // TODO launch time consuming operation
            lock.lock();
        }
    }

    void LogExporter::_Start()
    {
        this->_thread = new boost::thread(boost::bind(&LogExporter::_Run, this));
        this->_Notify();
    }

    void LogExporter::_Stop()
    {
        {
            boost::lock_guard<boost::mutex> lock(this->_mutex);
            this->_stopFlag = true;
        }
        this->_Notify();
        this->_thread->join();
        delete this->_thread;
    }

    void LogExporter::_Notify()
    {
        this->_cond.notify_one();
    }

    void LogExporter::_Run()
    {
        boost::unique_lock<boost::mutex> lock(this->_mutex);
        while (true)
        {
            this->_cond.wait(lock);
            if (this->_stopFlag)
            {
                this->_stopFlag = false;
                return;
            }
            this->_Work(lock);
        }
    }
}
