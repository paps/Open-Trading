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

#include "Signal.hpp"
#include "core/strategy/Strategy.hpp"

#define CLASS "[Core/Signal/Signal] "

namespace Core
{
    namespace Signal
    {
        Signal::Signal(Strategy::Strategy& strategy, std::string const& name, unsigned int minBars, bool triggerOnTick) :
            _strategy(strategy), _name(name), _minBars(minBars), _triggerOnTick(triggerOnTick)
        {
            this->_strategy.Log(CLASS "Signal \"" + this->_name + "\" instantiated.");
        }

        Signal::~Signal()
        {
            this->_strategy.Log(CLASS "Signal \"" + this->_name + "\" destroyed.");
        }

        void Signal::NotifyTradeStart(Controller::Status, float, float, float, float)
        {
        }

        void Signal::NotifyTradeTick(Bar const&, float, float, bool)
        {
        }

        void Signal::NotifyTradeSlUpdate(float)
        {
        }

        void Signal::NotifyTradeTpUpdate(float)
        {
        }

        void Signal::NotifyTradeStop()
        {
        }

        void Signal::ClearBars()
        {
            this->_bars.clear();
        }

        void Signal::AddBar(Bar const& bar)
        {
            this->_bars.push_front(bar);
            if (this->_bars.size() > MaxBars)
                this->_bars.pop_back();
        }

        std::string const& Signal::GetName() const
        {
            return this->_name;
        }

        unsigned int Signal::GetMinBars() const
        {
            return this->_minBars;
        }

        bool Signal::TriggerOnTick() const
        {
            return this->_triggerOnTick;
        }

        std::list<Bar> const& Signal::GetBars() const
        {
            return this->_bars;
        }

        Strategy::Strategy& Signal::GetStrategy()
        {
            return this->_strategy;
        }

        void Signal::Log(std::string const& msg, Logger::MessageType type /* = Logger::Info */)
        {
            this->_strategy.Log(msg, type);
        }
    }
}
