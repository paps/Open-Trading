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

#ifndef __CORE_STRATEGY_STRATEGY__
#define __CORE_STRATEGY_STRATEGY__

#include <boost/noncopyable.hpp>
#include <string>
#include "logger/Logger.hpp"

namespace Core
{
    namespace Signal
    {
        class Signal;
    }
    namespace Actor
    {
        class Actor;
    }
    class Feedback;

    namespace Strategy
    {
        class Strategy :
            private boost::noncopyable
        {
            public:
                /*
                   name -> Set by the implementation subclass.
                   logger -> Given by the implemented UI.
                   pair, period, digits -> Client setup received by the network (not changed by the subclass).
                 */
                explicit Strategy(std::string const& name,
                        Logger::Logger const& logger,
                        Feedback& feedback,
                        std::string const& pair,
                        unsigned int period,
                        unsigned int digits);

                virtual ~Strategy();

                /*
                   Guaranteed to be called when creating a strategy in StrategyFactory.
                   Must initialize _signal and _actor when returning true.
                   true -> Success. Deinit() is called when deleting the strategy in StrategyFactory.
                   false -> Failure. Deinit() will never be called, StrategyFactory will return 0.
                 */
                virtual bool Init() = 0;

                /*
                   Guaranteed to be called when deleting the strategy in StrategyFactory.
                 */
                virtual void Deinit() = 0;

                Signal::Signal& GetSignal();
                Actor::Actor& GetActor();
                std::string const& GetName() const;
                std::string const& GetPair() const;
                unsigned int GetPeriod() const;
                unsigned int GetDigits() const;
                float GetOffsetToPipRatio() const;
                float PipsToOffset(float pips) const;
                float OffsetToPips(float offset) const;
                float CeilPrice(float price) const;
                float FloorPrice(float price) const;
                float RoundPrice(float price) const;
                float GetPipPrice() const;
                float GetPriceUnit() const;
                void Log(std::string const& msg, Logger::MessageType type = Logger::Info);
                Logger::Logger const& GetLogger();
                Feedback& GetFeedback();
            protected:
                Signal::Signal* _signal;
                Actor::Actor* _actor;
            private:
                std::string _name;
                std::string _pair;
                unsigned int _period;
                unsigned int _digits;
                unsigned int _priceToInteger;
                float _pipPrice;
                float _priceUnit;
                float _offsetToPipRatio;
                Logger::Logger const& _logger;
                Feedback& _feedback;
        };
    };
}

#endif
