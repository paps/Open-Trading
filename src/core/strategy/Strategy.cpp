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
#include "Strategy.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/Strategy/Strategy] "

namespace Core
{
    namespace Strategy
    {
        Strategy::Strategy(std::string const& name,
                Logger::Logger const& logger,
                Feedback& feedback,
                std::string const& pair,
                unsigned int period,
                unsigned int digits) :
            _signal(0),
            _actor(0),
            _name(name),
            _pair(pair),
            _period(period),
            _digits(digits),
            _logger(logger),
            _feedback(feedback)
        {
            this->Log(CLASS "Strategy \"" + this->_name + "\" instantiated.");
            if (this->_pair.size() != 6)
            {
                this->Log(CLASS "Incorrect pair name \"" + pair + "\".", Logger::Warning);
                this->_pair = "_ERROR";
            }
            if (!this->_period)
            {
                this->Log(CLASS "Period of 0, changing to 1.", Logger::Warning);
                this->_period = 1;
            }
            if (this->_digits == 2)
                this->_priceUnit = 0.01;
            else if (this->_digits == 3)
                this->_priceUnit = 0.001;
            else if (this->_digits == 4)
                this->_priceUnit = 0.0001;
            else
                this->_priceUnit = 0.00001;
            if (this->_digits == 2 || this->_digits == 3)
            {
                this->_offsetToPipRatio = 100;
                this->_pipPrice = 0.01;
            }
            else if (this->_digits == 4 || this->_digits == 5)
            {
                this->_offsetToPipRatio = 10000;
                this->_pipPrice = 0.0001;
            }
            else
            {
                this->Log(CLASS "Weird number of digits: " + Tools::ToString(this->_digits) + ". Using default offset-to-pip-ratio: 10000.", Logger::Warning);
                this->_digits = 5;
                this->_offsetToPipRatio = 10000;
                this->_pipPrice = 0.0001;
            }
            this->_priceToInteger = pow(10, this->_digits);
        }

        Strategy::~Strategy()
        {
            this->Log(CLASS "Strategy \"" + this->_name + "\" destroyed.");
        }

        Signal::Signal& Strategy::GetSignal()
        {
            return *this->_signal;
        }

        Actor::Actor& Strategy::GetActor()
        {
            return *this->_actor;
        }

        std::string const& Strategy::GetName() const
        {
            return this->_name;
        }

        std::string const& Strategy::GetPair() const
        {
            return this->_pair;
        }

        unsigned int Strategy::GetPeriod() const
        {
            return this->_period;
        }

        unsigned int Strategy::GetDigits() const
        {
            return this->_digits;
        }

        float Strategy::GetOffsetToPipRatio() const
        {
            return this->_offsetToPipRatio;
        }

        float Strategy::PipsToOffset(float pips) const
        {
            return pips / this->_offsetToPipRatio;
        }

        float Strategy::OffsetToPips(float offset) const
        {
            return offset * this->_offsetToPipRatio;
        }

        float Strategy::CeilPrice(float price) const
        {
            return ceil(price * this->_priceToInteger) / this->_priceToInteger;
        }

        float Strategy::FloorPrice(float price) const
        {
            return floor(price * this->_priceToInteger) / this->_priceToInteger;
        }

        float Strategy::RoundPrice(float price) const
        {
            return floor(price * this->_priceToInteger + 0.5) / this->_priceToInteger;
        }

        float Strategy::GetPipPrice() const
        {
            return this->_pipPrice;
        }

        float Strategy::GetPriceUnit() const
        {
            return this->_priceUnit;
        }

        void Strategy::Log(std::string const& msg, Logger::MessageType type /* = Logger::Info */)
        {
            this->_logger.Log(msg, type);
        }

        Logger::Logger const& Strategy::GetLogger()
        {
            return this->_logger;
        }

        Feedback& Strategy::GetFeedback()
        {
            return this->_feedback;
        }
    }
}
