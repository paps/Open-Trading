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

#ifndef __BACKTESTER_TASK__
#define __BACKTESTER_TASK__

#include <boost/noncopyable.hpp>
#include <list>
#include "core/Controller.hpp"

namespace Core
{
    class StrategyInstantiator;
}

namespace Backtester
{
    class TickGenerator;
    class Logger;
    class Feedback;
    class StratParamsMap;
    class Conf;
    class Report;
    class PlotGenerator;

    class Task :
        private boost::noncopyable
    {
        public:
            explicit Task(TickGenerator& tickGenerator,
                    Logger const& logger,
                    Conf const& conf,
                    StratParamsMap& stratParams,
                    Report& report);
            ~Task();
            bool Run();
        private:
            struct State
            {
                Core::Controller::Status status;
                float open;
                float lots;
                float sl;
                float tp;
                float balance;
            };
            void _Run(Core::Controller& controller);
            void _ClosePosition(Core::Bar const& bar, std::pair<float, float> const& tick, std::string const& reason);
            void _ClosePosition(Core::Bar const& bar, float price, std::string const& reason);
            void _Interrupt(Core::Bar const& bar, std::pair<float, float> const& tick);
            void _PreTick(Core::Bar const& bar, std::pair<float, float> const& tick);
            bool _PostTick(Core::Bar const& bar, std::pair<float, float> const& tick, Core::Controller::Output const& output);
            void _ResetState();
            std::string _PriceString(std::pair<float, float> const& tick) const;
            std::string _PriceString(float price) const;
            bool _CheckPriceRange(std::pair<float, float> const& tick, float price) const;
            void _AddPlotData(time_t, std::pair<float, float> const& tick);
            TickGenerator& _tickGenerator;
            Logger const& _logger;
            Conf const& _conf;
            StratParamsMap& _stratParams;
            Feedback* _feedback;
            Core::StrategyInstantiator* _strategyInstantiator;
            State _state;
            Report& _report;
            PlotGenerator* _plotGenerator;
    };
}

#endif
