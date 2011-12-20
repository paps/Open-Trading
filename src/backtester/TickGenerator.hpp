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

#ifndef __BACKTESTER_TICKGENERATOR__
#define __BACKTESTER_TICKGENERATOR__

#include <boost/noncopyable.hpp>
#include <utility>
#include <queue>
#include "core/Bar.hpp"

namespace Core
{
    class History;
    namespace Strategy
    {
        class Strategy;
    }
}

namespace Backtester
{
    class Conf;
    class Logger;

    class TickGenerator :
        private boost::noncopyable
    {
        public:
            enum GenerationResult
            {
                NormalTick,
                NewBarTick,
                Interruption,
                NoMoreTicks,
            };
            explicit TickGenerator(Core::History const& history, Logger const& logger, Conf const& conf);

            /*
               tick.first -> ask, tick.second -> bid
               Return values:
                - NormalTick: bar updated, valid tick
                - NewBarTick: bar updated, valid tick, newBar must be true for the next call to ProcessTick()
                - Interruption: gap in history, tick not modified, bar not modified, must call Interrupt()
                - NoMoreTicks: no more bars in history, tick not modified, bar not modified
            */
            GenerationResult GenerateNextTick(Core::Strategy::Strategy const& strategy, std::pair<float, float>& tick, Core::Bar& bar);

        private:
            void _NextBar();
            void _GenerateTicks(Core::Strategy::Strategy const& strategy, Core::Bar const& bar);
            void _GenerateFewerTicks(Core::Strategy::Strategy const& strategy, Core::Bar const& bar);
            Core::History const& _history;
            Conf const& _conf;
            Logger const& _logger;
            unsigned int _historyPos;
            unsigned int _barPos;
            Core::Bar _currentBar;
            std::queue<float> _tickBuffer;
    };
}

#endif
