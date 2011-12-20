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

#include <boost/cstdlib.hpp>
#include "Logger.hpp"
#include "Backtester.hpp"
#include "conf/Conf.hpp"
#include "Conf.hpp"
#include "core/History.hpp"

int main(int, char**)
{
    // logger
    Backtester::Logger logger;

    // conf
    Conf::Conf conf("backtester.lua");
    if (conf.Error())
    {
        logger.Log("Failed to load configuration file: \"" + conf.GetLastError() + "\". Aborting.", Logger::Error);
        return boost::exit_failure;
    }
    Backtester::Conf copyableConf(conf, logger);

    // history
    Core::History history(logger);
    if (!history.Load(conf.Read<std::string>("history", ""), conf.Read<unsigned int>("maxGapSize", 60)))
    {
        logger.Log("Failed to load history, aborting.", Logger::Error);
        return boost::exit_failure;
    }

    // backtester
    Backtester::Backtester backtester(logger, copyableConf, history);
    backtester.Run();

    return boost::exit_success;
}
