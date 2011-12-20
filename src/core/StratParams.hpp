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

#ifndef __CORE_STRATPARAMS__
#define __CORE_STRATPARAMS__

#include <boost/noncopyable.hpp>
#include <string>
#include "logger/Logger.hpp"

namespace Core
{
    class StratParams :
        private boost::noncopyable
    {
        public:
            explicit StratParams(Logger::Logger const& logger);
            virtual ~StratParams();

            /*
               Voluntarily non-const getters.
            */
            virtual float GetFloat(std::string const& name, float defaultValue) = 0;
            virtual std::string GetString(std::string const& name, std::string const& defaultValue) = 0;

            void Log(std::string const& msg, Logger::MessageType type = Logger::Info) const;
        private:
            Logger::Logger const& _logger;
    };
}

#endif
