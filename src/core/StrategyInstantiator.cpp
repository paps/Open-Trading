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

#include "StrategyInstantiator.hpp"
#include "Feedback.hpp"
#include "StratParams.hpp"
#include "logger/Logger.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/strategy/MaCross.hpp"

#define CLASS "[Core/StrategyInstantiator] "

namespace Core
{
    StrategyInstantiator::StrategyInstantiator(Logger::Logger const& logger, Feedback& feedback, StratParams& stratParams) :
        _logger(logger), _feedback(feedback), _stratParams(stratParams), _strategy(0)
    {
    }

    StrategyInstantiator::~StrategyInstantiator()
    {
        if (this->StrategyInstantiated())
            this->Destroy();
    }

    bool StrategyInstantiator::Instantiate(std::string const& name, std::string const& pair, unsigned int period, unsigned int digits)
    {
        if (this->_strategy)
        {
            this->_logger.Log(CLASS "Could not instantiate strategy \"" + name + "\": \"" + this->_strategy->GetName() + "\" is already instantiated.", Logger::Warning);
            return false;
        }

        if (name == "MaCross")
            this->_strategy = new Strategy::MaCross(this->_logger, this->_feedback, pair, period, digits, this->_stratParams);
        // else if (name == "OtherStrategy")
        //     this->_strategy = new Strategy::OtherStrategy(...) // add more strategies here

        if (!this->_strategy)
        {
            this->_logger.Log(CLASS "Failed to create strategy \"" + name + "\".");
            return 0;
        }
        if (!this->_strategy->Init())
        {
            this->_logger.Log(CLASS "Failed to initialize strategy \"" + this->_strategy->GetName() + "\".");
            delete this->_strategy;
            this->_strategy = 0;
            return 0;
        }
        this->_logger.Log(CLASS "Strategy \"" + this->_strategy->GetName() + "\" initialized successfully.");
        if (this->_feedback.IsNeeded())
            this->_feedback.SetBarsInfo(pair, period, digits);
        return true;
    }

    Strategy::Strategy* StrategyInstantiator::GetStrategy() const
    {
        return this->_strategy;
    }

    bool StrategyInstantiator::StrategyInstantiated() const
    {
        return this->_strategy != 0;
    }

    void StrategyInstantiator::Destroy()
    {
        if (!this->_strategy)
        {
            this->_logger.Log(CLASS "Could not destroy strategy: no strategy instantiated.", Logger::Warning);
            return;
        }
        this->_logger.Log(CLASS "Destroying strategy \"" + this->_strategy->GetName() + "\".");
        this->_strategy->Deinit();
        delete this->_strategy;
        this->_strategy = 0;
        this->_feedback.Reset();
    }
}
