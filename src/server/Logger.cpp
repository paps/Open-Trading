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

#include "Logger.hpp"
#include "Ui.hpp"
#include "Cli.hpp"

namespace Server
{
    Logger::Logger(Ui& ui, bool logToUi, bool logToStdOut) :
        _ui(ui), _buffering(false), _logToUi(logToUi), _logToStdOut(logToStdOut)
    {
    }

    void Logger::Log(std::string const& msg, ::Logger::MessageType type /* = ::Logger::Info */) const
    {
        this->Log(QString::fromStdString(msg), type);
    }

    void Logger::Log(char const* msg, ::Logger::MessageType type /* = ::Logger::Info */) const
    {
        this->Log(QString(msg), type);
    }

    void Logger::Log(QString const& msg, ::Logger::MessageType type /* = ::Logger::Info */) const
    {
        if (this->_buffering)
        {
            BufferEntry* e = new BufferEntry;
            e->msg = msg;
            e->type = type;
            this->_buffer.push_back(e);
        }
        else
            this->_DoLog(msg, type);
    }

    void Logger::BeginBuffering()
    {
        this->_buffering = true;
    }

    void Logger::EndBuffering()
    {
        std::list<BufferEntry*>::iterator it = this->_buffer.begin();
        std::list<BufferEntry*>::iterator itEnd = this->_buffer.end();
        for (; it != itEnd; ++it)
        {
            this->_DoLog((*it)->msg, (*it)->type);
            delete *it;
        }
        this->_buffering = false;
        this->_buffer.clear();
    }

    void Logger::_DoLog(QString const& msg, ::Logger::MessageType type) const
    {
        if (this->_ui.IsGraphical())
        {
            if (this->_logToUi)
                this->_ui.Log(msg, type);
            if (this->_logToStdOut)
                Cli::LogToStdOut(msg, type);
        }
        else if (this->_logToUi)
            this->_ui.Log(msg, type);
    }
}
