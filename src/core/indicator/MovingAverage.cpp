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

#include "MovingAverage.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/StratParams.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/Indicator/MovingAverage] "

namespace Core
{
    namespace Indicator
    {
        MovingAverage::MovingAverage(Strategy::Strategy& strategy, unsigned int period, StratParams& stratParams) :
            Indicator(strategy, "MovingAverage", 1),
            _period(period),
            _debug(stratParams.GetString("maDebug", "normal") == "debug")
        {
            if (this->_period < 2)
            {
                this->Log(CLASS "Invalid period of " + Tools::ToString(this->_period) + ", chaging to 2.", Logger::Warning);
                this->_period = 2;
            }
        }

        void MovingAverage::Run()
        {
            std::list<Bar> const& bars = this->GetBars();
            if (bars.size() >= this->_period)
            {
                float total = 0;
                unsigned int i = 0;
                std::list<Bar>::const_iterator it = bars.begin();
                while (i < this->_period)
                {
                    if (this->_debug)
                        this->Log(CLASS "MA (" + Tools::ToString(this->_period) + ") value " + Tools::ToString(i + 1) + ": " + Tools::ToString(it->c, this->GetStrategy().GetDigits()));
                    total += it->c;
                    ++i;
                    ++it;
                }
                this->_SetValidity(true);
                this->_SetOutput(OutputMovingAverage, total / this->_period);
            }
            else
                this->_SetValidity(false);
        }

        unsigned int MovingAverage::GetPeriod() const
        {
            return this->_period;
        }
    }
}
