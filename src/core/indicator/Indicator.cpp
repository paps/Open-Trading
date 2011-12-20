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

#include "Indicator.hpp"
#include "core/signal/Signal.hpp"
#include "core/strategy/Strategy.hpp"

#define CLASS "[Core/Indicator/Indicator] "

namespace Core
{
    namespace Indicator
    {
        Indicator::Indicator(Strategy::Strategy& strategy, std::string const& name, unsigned int nbOutputs) :
            _strategy(strategy), _name(name), _valid(false)
        {
            if (nbOutputs < 1)
            {
                this->_strategy.Log(CLASS "Invalid number of outputs.", Logger::Warning);
                nbOutputs = 1;
            }
            this->_outputs.resize(nbOutputs);
        }

        std::list<Bar> const& Indicator::GetBars() const
        {
            return this->_strategy.GetSignal().GetBars();
        }

        unsigned int Indicator::GetNbOutputs() const
        {
            return this->_outputs.size();
        }

        void Indicator::_SetOutput(unsigned int key, float value)
        {
            if (key < this->_outputs.size())
                this->_outputs[key] = value;
            else
                this->_strategy.Log(CLASS "Invalid output key while setting value.", Logger::Warning);
        }

        float Indicator::GetOutput(unsigned int key) const
        {
            if (key < this->_outputs.size())
                return this->_outputs[key];
            this->_strategy.Log(CLASS "Invalid output key while getting value.", Logger::Warning);
            return 0;
        }

        std::vector<float> const& Indicator::GetOutputs() const
        {
            return this->_outputs;
        }

        bool Indicator::IsValid() const
        {
            return this->_valid;
        }

        void Indicator::_SetValidity(bool validity)
        {
            this->_valid = validity;
        }

        std::string const& Indicator::GetName() const
        {
            return this->_name;
        }

        Strategy::Strategy& Indicator::GetStrategy()
        {
            return this->_strategy;
        }

        void Indicator::Log(std::string const& msg, Logger::MessageType type /* = Logger::Info */)
        {
            this->_strategy.Log(msg, type);
        }
    }
}
