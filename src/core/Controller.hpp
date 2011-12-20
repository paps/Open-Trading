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

#ifndef __CORE_CONTROLLER__
#define __CORE_CONTROLLER__

#include <boost/noncopyable.hpp>
#include <string>
#include "Bar.hpp"

namespace Core
{
    namespace Strategy
    {
        class Strategy;
    }
    namespace Actor
    {
        class Actor;
    }

    class Controller :
        private boost::noncopyable
    {
        public:
            enum Status // (network protocol values)
            {
                StatusNothing = 0,
                StatusBuy = 1,
                StatusSell = 2,
                StatusUnknown = 3,
            };
            enum Order // (network protocol values)
            {
                OrderNothing = 0,
                OrderBuy = 1,
                OrderSell = 2,
                OrderClose = 3,
                OrderAdjust = 4,
            };
            struct Output
            {
                Order order;
                float lots;
                float sl;
                float tp;
            };
            explicit Controller(Strategy::Strategy& strategy);
            Output const& GetLastOutput();
            void Interrupt();
            void ProcessTick(Bar const& bar, float ask, float bid, Status status, bool newBar);
            void ProcessTrade(Status status, float open, float lots, float sl, float tp, float askRequote, float bidRequote);
            void ProcessBar(Bar const& bar);
            Strategy::Strategy& GetStrategy();
            static char const* ToString(Status status);
            static char const* ToString(Order order);
        private:
            void _ResetOutput();
            Output _output;
            Strategy::Strategy& _strategy;
            bool _actorEnabled;
            Bar _lastBar;
    };
}

#endif
