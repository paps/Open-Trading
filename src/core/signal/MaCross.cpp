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

#include "MaCross.hpp"
#include "core/StratParams.hpp"
#include "core/strategy/Strategy.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/Signal/MaCross] "

namespace Core
{
    namespace Signal
    {
        MaCross::MaCross(Strategy::Strategy& strategy, StratParams& stratParams) :
            Signal(strategy, "MaCross", stratParams.GetFloat("macSlowMa", 100), false),
            _fastMa(strategy, stratParams.GetFloat("macFastMa", 20), stratParams),
            _slowMa(strategy, stratParams.GetFloat("macSlowMa", 100), stratParams),
            _prevFastMa(-1),
            _prevSlowMa(-1),
            _lots(stratParams.GetFloat("macLots", 0.01)),
            _sl(stratParams.GetFloat("macSl", 10)),
            _tp(stratParams.GetFloat("macTp", 10)),
            _debug(stratParams.GetString("macDebug", "normal") == "debug")
        {
        }

        void MaCross::Run(Controller::Output& output, Bar const&, float ask, float bid, bool)
        {
            this->_fastMa.Run();
            this->_slowMa.Run();
            if (this->_fastMa.IsValid() && this->_slowMa.IsValid())
            {
                float fastMa = this->_fastMa.GetOutput(Indicator::MovingAverage::OutputMovingAverage);
                float slowMa = this->_slowMa.GetOutput(Indicator::MovingAverage::OutputMovingAverage);
                if (this->_debug)
                    this->Log(CLASS "fastMa: " + Tools::ToString(fastMa) + ", slowMa: " + Tools::ToString(slowMa) + ", " + (fastMa > slowMa ? "fastMa" : "slowMa"));
                if (this->_prevFastMa >= 0 && this->_prevSlowMa >= 0)
                {
                    if (this->_prevFastMa > this->_prevSlowMa && fastMa < slowMa)
                    {
                        output.order = Core::Controller::OrderBuy;
                        output.lots = this->_lots;
                        output.sl = ask - this->GetStrategy().PipsToOffset(this->_sl);
                        output.tp = ask + this->GetStrategy().PipsToOffset(this->_tp);
                    }
                    else if (this->_prevFastMa < this->_prevSlowMa && fastMa > slowMa)
                    {
                        output.order = Core::Controller::OrderSell;
                        output.lots = this->_lots;
                        output.sl = bid + this->GetStrategy().PipsToOffset(this->_sl);
                        output.tp = bid - this->GetStrategy().PipsToOffset(this->_tp);
                    }
                }
                this->_prevFastMa = fastMa;
                this->_prevSlowMa = slowMa;
            }
            else
            {
                this->_prevFastMa = -1;
                this->_prevSlowMa = -1;
            }
        }

        void MaCross::NotifyTradeStop()
        {
            this->_prevFastMa = -1;
            this->_prevSlowMa = -1;
        }
    }
}
