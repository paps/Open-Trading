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

#include "Controller.hpp"
#include "Feedback.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/signal/Signal.hpp"
#include "core/actor/Actor.hpp"

#define CLASS "[Core/Controller] "

namespace Core
{
    Controller::Controller(Strategy::Strategy& strategy) :
        _strategy(strategy), _actorEnabled(false)
    {
    }

    void Controller::_ResetOutput()
    {
        this->_output.order = OrderNothing;
        this->_output.lots = -1;
        this->_output.sl = -1;
        this->_output.tp = -1;
    }

    Controller::Output const& Controller::GetLastOutput()
    {
        return this->_output;
    }

    void Controller::Interrupt()
    {
        if (this->_strategy.GetActor().IsEnabled())
            this->_strategy.GetActor().Stop();
        this->_strategy.GetSignal().ClearBars();
        // Feedback
        // XXX bars handling
    }

    void Controller::ProcessTick(Bar const& bar, float ask, float bid, Status status, bool newBar)
    {
        this->_ResetOutput(); // this is the only packet which results in an output
        // Tick packet processing
        if (newBar && this->_lastBar.valid)
            this->_strategy.GetSignal().AddBar(this->_lastBar);
        this->_lastBar = bar;
        if (!this->_lastBar.valid)
        {
            this->_strategy.Log(CLASS "Invalid bar received while processing tick.", Logger::Warning);
            this->_lastBar.valid = true;
        }
        if (this->_strategy.GetActor().IsEnabled()) // already trading
        {
            if (status != StatusNothing)
            {
                if (this->_strategy.GetActor().GetStatus() != status) // should never happen
                {
                    this->_output.order = OrderClose;
                    this->_strategy.Log(CLASS "Tick packet with a different status than the actor.", Logger::Error);
                }
                else
                {
                    this->_strategy.GetActor().Run(this->_output, bar, ask, bid, newBar);
                    this->_strategy.GetSignal().NotifyTradeTick(bar, ask, bid, newBar);
                }
            }
            else
                this->_strategy.GetActor().Stop(); // the client closed the position
        }
        else // not trading
        {
            if (status != StatusNothing)
                this->_strategy.Log(CLASS "Tick packet with an active status but the actor is not enabled.", Logger::Warning);
            if (newBar || this->_strategy.GetSignal().TriggerOnTick()) // trigger signal on new bar or on tick
                this->_strategy.GetSignal().Run(this->_output, bar, ask, bid, newBar);
        }
        // Feedback
        if (this->_strategy.GetFeedback().IsNeeded())
        {
            this->_strategy.GetFeedback().SetMarketInfo(
                    ask,
                    bid,
                    this->_strategy.OffsetToPips(ask - bid),
                    bar,
                    newBar ? 1 : this->_strategy.GetFeedback().GetMarketInfo().ticks + 1);
            if (this->_strategy.GetActor().IsEnabled())
            {
                Feedback::State::Position const& pos = this->_strategy.GetFeedback().GetPositionInfo();
                this->_strategy.GetFeedback().SetPositionInfo(
                        pos.status,
                        pos.open,
                        pos.lots,
                        pos.sl,
                        pos.tp,
                        this->_strategy.OffsetToPips(pos.status == Core::Controller::StatusBuy ? bid - pos.open : pos.open - ask));
            }
        }
    }

    void Controller::ProcessTrade(Status status, float open, float lots, float sl, float tp, float askRequote, float bidRequote)
    {
        // Trade packet processing
        if (status == StatusUnknown) // unknown status
            this->_strategy.Log(CLASS "Trade packet with an unknown status.", Logger::Warning);
        else // known status (buy, sell or nothing)
        {
            if (this->_strategy.GetActor().IsEnabled()) // already trading
            {
                if (status != StatusBuy && status != StatusSell) // not a buy or a sell (normal actor stop)
                    this->_strategy.GetActor().Stop();
                else // currently trading (buy or sell)
                {
                    if (this->_strategy.GetActor().GetStatus() != status) // should never happen
                        this->_strategy.Log(CLASS "Trade packet with a different status than the actor.", Logger::Error);
                    if (this->_strategy.GetActor().GetSl() != sl) // sl changed
                        this->_strategy.GetActor().UpdateSl(sl);
                    if (this->_strategy.GetActor().GetTp() != tp) // tp changed
                        this->_strategy.GetActor().UpdateTp(tp);
                }
            }
            else // not trading
            {
                if (status != StatusBuy && status != StatusSell) // not a buy or a sell
                    this->_strategy.Log(CLASS "Trade packet with no opened position.", Logger::Warning);
                else // buy or sell status
                    this->_strategy.GetActor().Start(status, open, lots, sl, tp, askRequote, bidRequote);
            }
        }
    }

    void Controller::ProcessBar(Bar const& bar)
    {
        // Bar packet processing
        this->_strategy.GetSignal().AddBar(bar);
        // Feedback
        // XXX bars handling
    }

    Strategy::Strategy& Controller::GetStrategy()
    {
        return this->_strategy;
    }

    char const* Controller::ToString(Status status)
    {
        switch (status)
        {
            case StatusNothing:
                return "not active";
            case StatusBuy:
                return "buy";
            case StatusSell:
                return "sell";
            case StatusUnknown:
                return "unknown";
            default:
                return "invalid status";
        }
    }

    char const* Controller::ToString(Order order)
    {
        switch (order)
        {
            case OrderNothing:
                return "nothing";
            case OrderBuy:
                return "buy";
            case OrderSell:
                return "sell";
            case OrderClose:
                return "close";
            case OrderAdjust:
                return "adjust";
            default:
                return "invalid order";
        }
    }
}
