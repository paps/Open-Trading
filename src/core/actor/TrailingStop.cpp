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

#include "TrailingStop.hpp"
#include "core/StratParams.hpp"
#include "core/strategy/Strategy.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/Actor/TrailingStop] "

namespace Core
{
    namespace Actor
    {
        TrailingStop::TrailingStop(Strategy::Strategy& strategy, StratParams& stratParams) :
            Actor(strategy, "TrailingStop"),
            _debug(stratParams.GetString("tsLog", "normal") == "debug"),
            _distance(stratParams.GetFloat("tsDistance", 5))
        {
        }

        bool TrailingStop::_Start()
        {
            if (this->GetStatus() == Core::Controller::StatusBuy)
            {
                this->_stop = this->GetBidRequote() - this->GetStrategy().PipsToOffset(this->_distance);
                if (this->_debug)
                    this->Log(CLASS "Initial buy SL " + Tools::ToString(this->_stop, this->GetStrategy().GetDigits()) + ".");
            }
            else
            {
                this->_stop = this->GetAskRequote() + this->GetStrategy().PipsToOffset(this->_distance);
                if (this->_debug)
                    this->Log(CLASS "Initial sell SL " + Tools::ToString(this->_stop, this->GetStrategy().GetDigits()) + ".");
            }
            return true;
        }

        void TrailingStop::_Stop()
        {
        }

        void TrailingStop::Run(Controller::Output& output, Bar const&, float ask, float bid, bool)
        {
            if (this->GetStatus() == Core::Controller::StatusBuy) // buy
            {
                if (bid >= this->GetOpen() + this->_tp) // TP hit
                {
                    output.order = Controller::OrderClose;
                    if (this->_debug)
                        this->Log(CLASS "Buy TP hit.");
                }
                else
                {
                    if (bid - this->GetStrategy().PipsToOffset(this->_distance) > this->_stop) // good
                    {
                        this->_stop = bid - this->GetStrategy().PipsToOffset(this->_distance);
                        if (this->_debug)
                            this->Log(CLASS "Raising buy SL " + Tools::ToString(this->_stop, this->GetStrategy().GetDigits()) + ".");
                    }
                    else if (bid <= this->_stop) // bad
                    {
                        output.order = Controller::OrderClose;
                        if (this->_debug)
                            this->Log(CLASS "Buy SL hit.");
                    }
                }
            }
            else // sell
            {
                if (ask <= this->GetOpen() - this->_tp) // TP hit
                {
                    output.order = Controller::OrderClose;
                    if (this->_debug)
                        this->Log(CLASS "Sell TP hit.");
                }
                else
                {
                    if (ask + this->GetStrategy().PipsToOffset(this->_distance) < this->_stop) // good
                    {
                        this->_stop = ask + this->GetStrategy().PipsToOffset(this->_distance);
                        if (this->_debug)
                            this->Log(CLASS "Lowering sell SL " + Tools::ToString(this->_stop, this->GetStrategy().GetDigits()) + ".");
                    }
                    else if (ask >= this->_stop) // bad
                    {
                        output.order = Controller::OrderClose;
                        if (this->_debug)
                            this->Log(CLASS "Sell SL hit.");
                    }
                }
            }
        }

        void TrailingStop::_UpdateSl()
        {
        }

        void TrailingStop::_UpdateTp()
        {
        }
    }
}
