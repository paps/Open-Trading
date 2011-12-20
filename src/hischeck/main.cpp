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
#include "tools/ToString.hpp"
#include "core/History.hpp"
#include "core/Bar.hpp"

int main(int ac, char** av)
{
    Hischeck::Logger logger;

    if (ac <= 1 || !av[1])
    {
        logger.Log("Usage: hischeck PATH_TO_CSV");
        return boost::exit_failure;
    }

    Core::History history(logger, true);
    if (!history.Load(av[1]))
    {
        logger.Log("Failed to load history.", Logger::Error);
        return boost::exit_failure;
    }

    std::vector<Core::Bar> const& bars = history.GetBars();
    logger.Log("Start:      " + bars[0].TimeToString() + ".");
    logger.Log("End:        " + bars[bars.size() - 1].TimeToString() + ".");
    logger.Log("Duration:");
    time_t diff = bars[bars.size() - 1].time - bars[0].time;
    logger.Log("   seconds: " + Tools::ToString(diff));
    logger.Log("   minutes: " + Tools::ToString(diff / 60));
    logger.Log("   hours:   " + Tools::ToString(diff / (60 * 60)));
    logger.Log("   days:    " + Tools::ToString(diff / (60 * 60 * 24)));
    return boost::exit_success;
}
