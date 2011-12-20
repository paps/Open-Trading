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

#include "Feedback.hpp"

namespace Core
{
    Feedback::Feedback()
    {
        this->Reset();
    }

    Feedback::State::Market const& Feedback::GetMarketInfo() const
    {
        return this->_state.market;
    }

    Feedback::State::Bars const& Feedback::GetBarsInfo() const
    {
        return this->_state.bars;
    }

    Feedback::State::Position const& Feedback::GetPositionInfo() const
    {
        return this->_state.position;
    }

    void Feedback::SetMarketInfo(float ask, float bid, float spd, Core::Bar const& bar, unsigned int ticks)
    {
        this->_state.market.ask = ask;
        this->_state.market.bid = bid;
        this->_state.market.spd = spd;
        this->_state.market.bar = bar;
        this->_state.market.ticks = ticks;
        this->_state.market.dirty = true;
    }

    void Feedback::SetBarsInfo(std::string pair, unsigned int period, unsigned int digits)
    {
        this->_state.bars.pair = pair;
        this->_state.bars.period = period;
        this->_state.bars.digits = digits;
        this->_state.bars.dirty = true;
    }

    void Feedback::SetPositionInfo(Core::Controller::Status status, float open, float lots, float sl, float tp, float pips)
    {
        this->_state.position.status = status;
        this->_state.position.open = open;
        this->_state.position.lots = lots;
        this->_state.position.sl = sl;
        this->_state.position.tp = tp;
        this->_state.position.pips = pips;
        this->_state.position.dirty = true;
    }

    void Feedback::Wash()
    {
        this->_state.market.dirty = false;
        this->_state.bars.dirty = false;
        this->_state.position.dirty = false;
    }

    void Feedback::Reset()
    {
        this->SetMarketInfo(-1, -1, -1, Core::Bar(-1, -1, -1, -1), 0);
        this->SetBarsInfo("-", -1, -1);
        this->SetPositionInfo(Core::Controller::StatusNothing, -1, -1, -1, -1, -1);
    }
}
