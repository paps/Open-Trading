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

#include "Actor.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/signal/Signal.hpp"
#include "core/Feedback.hpp"

#define CLASS "[Core/Actor/Actor] "

namespace Core
{
    namespace Actor
    {
        Actor::Actor(Strategy::Strategy& strategy, std::string const& name) :
            _strategy(strategy), _name(name), _logStartStop(true)
        {
            this->Log(CLASS "Actor \"" + this->_name + "\" instantiated.");
            this->_Disable();
        }

        Actor::~Actor()
        {
            this->_strategy.Log(CLASS "Actor \"" + this->_name + "\" destroyed.");
        }

        void Actor::SetLogStartStop(bool logStartStop)
        {
            this->_logStartStop = logStartStop;
        }

        void Actor::Start(Controller::Status status, float open, float lots, float sl, float tp, float askRequote, float bidRequote)
        {
            if (status != Controller::StatusBuy && status != Controller::StatusSell)
            {
                this->Log(CLASS "Failed to start actor because status is \"" + std::string(Controller::ToString(status)) + "\".", Logger::Warning);
                return;
            }
            this->_enabled = true;
            this->_status = status;
            this->_open = open;
            this->_lots = lots;
            this->_sl = sl;
            this->_tp = tp;
            this->_askRequote = askRequote;
            this->_bidRequote = bidRequote;
            if (this->_Start())
            {
                if (this->_logStartStop)
                    this->Log(CLASS "Successfully started actor \"" + this->_name + "\".");
            }
            else
            {
                this->Log(CLASS "Failed to start actor \"" + this->_name + "\".", Logger::Warning);
                this->_Disable();
            }
            this->_strategy.GetSignal().NotifyTradeStart(status, open, lots, sl, tp);
            if (this->_strategy.GetFeedback().IsNeeded())
                this->_strategy.GetFeedback().SetPositionInfo(status, open, lots, sl, tp, -1);
        }

        void Actor::Stop()
        {
            this->_Stop();
            this->_Disable();
            if (this->_logStartStop)
                this->Log(CLASS "Actor \"" + this->_name + "\" stopped.");
            this->_strategy.GetSignal().NotifyTradeStop();
            if (this->_strategy.GetFeedback().IsNeeded())
                this->_strategy.GetFeedback().SetPositionInfo(Core::Controller::StatusNothing, -1, -1, -1, -1, -1);
        }

        void Actor::UpdateSl(float sl)
        {
            this->_sl = sl;
            this->_UpdateSl();
            this->_strategy.GetSignal().NotifyTradeSlUpdate(sl);
            if (this->_strategy.GetFeedback().IsNeeded())
            {
                Feedback::State::Position const& pos = this->_strategy.GetFeedback().GetPositionInfo();
                this->_strategy.GetFeedback().SetPositionInfo(pos.status, pos.open, pos.lots, this->_sl, pos.tp, pos.pips);
            }
        }

        void Actor::UpdateTp(float tp)
        {
            this->_tp = tp;
            this->_UpdateTp();
            this->_strategy.GetSignal().NotifyTradeTpUpdate(tp);
            if (this->_strategy.GetFeedback().IsNeeded())
            {
                Feedback::State::Position const& pos = this->_strategy.GetFeedback().GetPositionInfo();
                this->_strategy.GetFeedback().SetPositionInfo(pos.status, pos.open, pos.lots, pos.sl, this->_tp, pos.pips);
            }
        }

        Controller::Status Actor::GetStatus() const
        {
            return this->_status;
        }

        Strategy::Strategy& Actor::GetStrategy()
        {
            return this->_strategy;
        }

        std::list<Bar> const& Actor::GetBars() const
        {
            return this->_strategy.GetSignal().GetBars();
        }

        float Actor::GetOpen() const
        {
            return this->_open;
        }

        float Actor::GetLots() const
        {
            return this->_lots;
        }

        float Actor::GetSl() const
        {
            return this->_sl;
        }

        float Actor::GetTp() const
        {
            return this->_tp;
        }

        float Actor::GetAskRequote() const
        {
            return this->_askRequote;
        }

        float Actor::GetBidRequote() const
        {
            return this->_bidRequote;
        }

        bool Actor::IsEnabled() const
        {
            return this->_enabled;
        }

        std::string const& Actor::GetName() const
        {
            return this->_name;
        }

        void Actor::_Disable()
        {
            this->_enabled = false;
            this->_status = Controller::StatusNothing;
            this->_open = -1;
            this->_lots = -1;
            this->_sl = -1;
            this->_tp = -1;
            this->_askRequote = -1;
            this->_bidRequote = -1;
            this->_enabled = false;
        }

        void Actor::Log(std::string const& msg, Logger::MessageType type /* = Logger::Info */)
        {
            this->_strategy.Log(msg, type);
        }
    }
}
