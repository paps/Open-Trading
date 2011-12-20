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

#include <cmath>
#include "TickGenerator.hpp"
#include "core/History.hpp"
#include "core/strategy/Strategy.hpp"
#include "Conf.hpp"
#include "Logger.hpp"

#define CLASS "[Backtester/TickGenerator] "

namespace Backtester
{
    TickGenerator::TickGenerator(Core::History const& history, Logger const& logger, Conf const& conf) :
        _history(history), _conf(conf), _logger(logger), _barPos(0)
    {
        this->_historyPos = this->_history.GetFirstBarPosOfPeriod(this->_conf.period);
    }

    TickGenerator::GenerationResult TickGenerator::GenerateNextTick(Core::Strategy::Strategy const& strategy, std::pair<float, float>& tick, Core::Bar& bar)
    {
        if (this->_tickBuffer.empty())
        {
            Core::Bar minuteBar;
            Core::History::FetchType fetch = this->_history.FetchBar(minuteBar, this->_historyPos, 1);
            if (fetch == Core::History::FetchError)
                return NoMoreTicks;
            else if (fetch == Core::History::FetchGap)
            {
                this->_NextBar();
                return Interruption;
            }
            this->_currentBar.time = minuteBar.time;
            if (this->_conf.fewerTicks)
                this->_GenerateFewerTicks(strategy, minuteBar);
            else
                this->_GenerateTicks(strategy, minuteBar);
        }
        float value = this->_tickBuffer.front();
        this->_tickBuffer.pop();
        GenerationResult ret;
        if (this->_currentBar.valid)
        {
            if (this->_currentBar.h < value)
                this->_currentBar.h = value;
            if (this->_currentBar.l > value)
                this->_currentBar.l = value;
            this->_currentBar.c = value;
            ret = NormalTick;
        }
        else
        {
            this->_currentBar.valid = true;
            this->_currentBar.o = value;
            this->_currentBar.h = value;
            this->_currentBar.l = value;
            this->_currentBar.c = value;
            ret = NewBarTick;
        }
        bar = this->_currentBar;
        ++this->_currentBar.time; // adds 1 second so that the next tick is not a multiple of 60
        tick.first = value + strategy.PipsToOffset(this->_conf.spread);
        tick.second = value;
        if (this->_tickBuffer.empty())
            this->_NextBar();
        return ret;
    }

    void TickGenerator::_NextBar()
    {
        ++this->_historyPos;
        ++this->_barPos;
        if (this->_barPos >= this->_conf.period)
        {
            this->_barPos = 0;
            this->_currentBar.valid = false;
        }
    }

    void TickGenerator::_GenerateFewerTicks(Core::Strategy::Strategy const&, Core::Bar const& bar)
    {
        this->_tickBuffer.push(bar.o);
        if (bar.o == bar.c)
        {
            if (bar.h == bar.l)
                return;
            else if (bar.l == bar.o)
                this->_tickBuffer.push(bar.h);
            else if (bar.h == bar.o)
                this->_tickBuffer.push(bar.l);
            else
            {
                this->_tickBuffer.push(bar.l);
                this->_tickBuffer.push(bar.h);
            }
        }
        else if (bar.l == bar.c && bar.h != bar.o)
            this->_tickBuffer.push(bar.h);
        else if (bar.h == bar.c && bar.l != bar.o)
            this->_tickBuffer.push(bar.l);
        else if (bar.o == bar.l)
            this->_tickBuffer.push(bar.h);
        else if (bar.o == bar.h)
            this->_tickBuffer.push(bar.l);
        else
        {
            if (bar.c > bar.o)
            {
                this->_tickBuffer.push(bar.l);
                this->_tickBuffer.push(bar.h);
            }
            else
            {
                this->_tickBuffer.push(bar.h);
                this->_tickBuffer.push(bar.l);
            }
        }
        this->_tickBuffer.push(bar.c);
    }

    void TickGenerator::_GenerateTicks(Core::Strategy::Strategy const& strategy, Core::Bar const& bar)
    {
        this->_tickBuffer.push(bar.o);
        if (bar.o == bar.c)
        {
            if (bar.h == bar.l) // l | h ~
                return;
            else if (bar.l == bar.o) // l |- h ~
                this->_tickBuffer.push(bar.h);
            else if (bar.h == bar.o) // l -| h ~
                this->_tickBuffer.push(bar.l);
            else // l -|- h ~
            {
                this->_tickBuffer.push(bar.l);
                this->_tickBuffer.push(bar.h);
            }
        }
        else if (bar.l == bar.c)
        {
            if (bar.h == bar.o) // l | | h <
                this->_tickBuffer.push(strategy.FloorPrice(bar.c + 0.5 * (bar.o - bar.c)));
            else // l | |- h <
                this->_tickBuffer.push(bar.h);
        }
        else if (bar.h == bar.c)
        {
            if (bar.l == bar.o) // l | | h >
                this->_tickBuffer.push(strategy.CeilPrice(bar.o + 0.5 * (bar.c - bar.o)));
            else // l -| | h >
                this->_tickBuffer.push(bar.l);
        }
        else if (bar.o == bar.l) // l | |- h >
            this->_tickBuffer.push(bar.h);
        else if (bar.o == bar.h) // l -| | h <
            this->_tickBuffer.push(bar.l);
        else
        {
            float unit = fabs(bar.c - bar.o) > strategy.GetPriceUnit() ? strategy.GetPriceUnit() : 0;
            if (bar.c > bar.o) // l -| |- h >
            {
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.25 * (bar.o - bar.l)));
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.5 * (bar.o - bar.l)));
                this->_tickBuffer.push(bar.l);
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.33 * (bar.h - bar.l)));
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.33 * (bar.h - bar.l) - unit));
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.66 * (bar.h - bar.l)));
                this->_tickBuffer.push(strategy.CeilPrice(bar.l + 0.66 * (bar.h - bar.l) - unit));
                this->_tickBuffer.push(bar.h);
                this->_tickBuffer.push(strategy.CeilPrice(bar.h - 0.75 * (bar.h - bar.c)));
                this->_tickBuffer.push(strategy.CeilPrice(bar.h - 0.5 * (bar.h - bar.c)));
            }
            else // l -| |- h <
            {
                this->_tickBuffer.push(strategy.FloorPrice(bar.h - 0.25 * (bar.h - bar.o)));
                this->_tickBuffer.push(strategy.FloorPrice(bar.h - 0.5 * (bar.h - bar.o)));
                this->_tickBuffer.push(bar.h);
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.66 * (bar.h - bar.l)));
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.66 * (bar.h - bar.l) + unit));
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.33 * (bar.h - bar.l)));
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.33 * (bar.h - bar.l) + unit));
                this->_tickBuffer.push(bar.l);
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.75 * (bar.c - bar.l)));
                this->_tickBuffer.push(strategy.FloorPrice(bar.l + 0.5 * (bar.c - bar.l)));
            }
        }
        this->_tickBuffer.push(bar.c);
    }
}
