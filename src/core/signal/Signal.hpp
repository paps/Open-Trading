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

#ifndef __CORE_SIGNAL_SIGNAL__
#define __CORE_SIGNAL_SIGNAL__

#include <boost/noncopyable.hpp>
#include <list>
#include "core/Controller.hpp"
#include "logger/Logger.hpp"

namespace Core
{
    namespace Signal
    {
        class Signal :
            private boost::noncopyable
        {
            public:
                explicit Signal(Strategy::Strategy& strategy, std::string const& name, unsigned int minBars, bool triggerOnTick);
                virtual ~Signal();

                /*
                   Process the added bars to produce a signal.
                   The result is given by modifying the controller output.
                   The last OHLC given by the client is currentBar.
                   The last completed bar of the strategy's period is at the front of _bars.
                   Called when a new bar is added, or on every tick if triggerOnTick was set.
                 */
                virtual void Run(Controller::Output& output, Bar const& currentBar, float ask, float bid, bool newBar) = 0;

                /*
                   Notifications methods, useful to be aware of what the actor is doing.
                   The base implementation does nothing.
                */
                virtual void NotifyTradeStart(Controller::Status status, float open, float lots, float sl, float tp);
                virtual void NotifyTradeTick(Bar const& currentBar, float ask, float bid, bool newBar);
                virtual void NotifyTradeSlUpdate(float sl);
                virtual void NotifyTradeTpUpdate(float tp);
                virtual void NotifyTradeStop();

                void AddBar(Bar const& bar);
                void ClearBars();
                std::string const& GetName() const;
                unsigned int GetMinBars() const;
                bool TriggerOnTick() const;
                std::list<Bar> const& GetBars() const;
                Strategy::Strategy& GetStrategy();
                void Log(std::string const& msg, Logger::MessageType type = Logger::Info);
            private:
                enum
                {
                    MaxBars = 2880, // 2 days in 1 minute bars
                };
                Strategy::Strategy& _strategy;
                std::string _name;
                unsigned int _minBars;
                bool _triggerOnTick;
                std::list<Bar> _bars;
        };
    }
}

#endif
