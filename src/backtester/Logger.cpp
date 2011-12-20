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

#include <iostream>
#include "Logger.hpp"
#include "tools/ToString.hpp"

namespace Backtester
{
    Logger::Logger(unsigned int id /* = 0 */) :
        _id(id)
    {
        if (this->_id)
            this->_prefix = "[Thread " + Tools::ToString(id) + "] ";
        else
            this->_prefix = "[Main] ";
    }

    unsigned int Logger::GetId() const
    {
        return this->_id;
    }

    void Logger::Log(std::string const& msg, ::Logger::MessageType type /* = ::Logger::Info */) const
    {
        if (type != ::Logger::Info)
        {
            if (type == ::Logger::Warning)
                std::cout << "[W] ";
            else
                std::cerr << "[E] ";
        }
        if (type != ::Logger::Info && type != ::Logger::Warning)
            std::cerr << this->_prefix << msg << std::endl;
        else
            std::cout << this->_prefix << msg << std::endl;
    }
}
