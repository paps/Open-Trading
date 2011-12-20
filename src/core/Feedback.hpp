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

#ifndef __CORE_FEEDBACK__
#define __CORE_FEEDBACK__

#include <boost/noncopyable.hpp>
#include <string>
#include "Controller.hpp"

namespace Core
{
    class Feedback :
        private boost::noncopyable
    {
        public:
            struct State
            {
                struct Market
                {
                    float ask;
                    float bid;
                    float spd;
                    Core::Bar bar;
                    unsigned int ticks;
                    bool dirty;
                };
                Market market;
                struct Bars
                {
                    std::string pair;
                    unsigned int period;
                    unsigned int digits;
                    bool dirty;
                };
                Bars bars;
                struct Position
                {
                    Core::Controller::Status status;
                    float pips;
                    float lots;
                    float open;
                    float sl;
                    float tp;
                    bool dirty;
                };
                Position position;
            };

            explicit Feedback();

            /*
             * No feedback is necessary if false is returned (implementation defined).
             */
            virtual bool IsNeeded() const = 0;

            /*
             * Sets all dirty flags to false.
             */
            void Wash();

            /*
             * Invalidates all infos & sets all dirty flags to true.
             */
            void Reset();

            /*
             * Market info: last bar, last prices & number of ticks.
             */
            State::Market const& GetMarketInfo() const;
            void SetMarketInfo(float ask, float bid, float spd, Core::Bar const& bar, unsigned int ticks);

            /*
             * Bars info: pair, period & digits.
             */
            State::Bars const& GetBarsInfo() const;
            void SetBarsInfo(std::string pair, unsigned int period, unsigned int digits);

            /*
             * Position info: status, gains/losses, open, sl & tp prices.
             */
            State::Position const& GetPositionInfo() const;
            void SetPositionInfo(Core::Controller::Status status, float open, float lots, float sl, float tp, float pips);

        private:
            State _state;
    };
}

#endif
