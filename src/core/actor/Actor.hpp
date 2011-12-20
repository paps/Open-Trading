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

#ifndef __CORE_ACTOR_ACTOR__
#define __CORE_ACTOR_ACTOR__

#include <boost/noncopyable.hpp>
#include <list>
#include "core/Controller.hpp"
#include "logger/Logger.hpp"

namespace Core
{
    namespace Strategy
    {
        class Strategy;
    }

    namespace Actor
    {
        class Actor :
            private boost::noncopyable
        {
            public:
                explicit Actor(Strategy::Strategy& strategy, std::string const& name);
                virtual ~Actor();
                void Start(Controller::Status status, float open, float lots, float sl, float tp, float askRequote, float bidRequote);
                void Stop();
                void UpdateSl(float sl);
                void UpdateTp(float tp);
                bool IsEnabled() const;
                std::string const& GetName() const;
                Strategy::Strategy& GetStrategy();
                void Log(std::string const& msg, Logger::MessageType type = Logger::Info);
                std::list<Bar> const& GetBars() const;
                void SetLogStartStop(bool logStartStop);

                /*
                   Getters for the subclass.
                 */
                Controller::Status GetStatus() const;
                float GetOpen() const;
                float GetLots() const;
                float GetSl() const;
                float GetTp() const;
                float GetAskRequote() const;
                float GetBidRequote() const;

                /*
                   New tick while trading.
                   Guaranteed to be called between _Start() and _Stop().
                 */
                virtual void Run(Controller::Output& output, Bar const& bar, float ask, float bid, bool newBar) = 0;

            private:

                /*
                   Start of trading, position opened. GetStatus() is guaranteed to return StatusBuy or StatusSell.
                   true -> Success: Run(), _UpdateSl() and _UpdateTp() will be called multiple times and _Stop() will be called at the end.
                   false -> Failure: _Stop(), Run(), _UpdateSl() and _UpdateTp() will not be called.
                 */
                virtual bool _Start() = 0;

                /*
                   End of trading, position closed.
                 */
                virtual void _Stop() = 0;

                /*
                   SL changed while trading.
                   Guaranteed to be called between _Start() and _Stop().
                */
                virtual void _UpdateSl() = 0;

                /*
                   TP changed while trading.
                   Guaranteed to be called between _Start() and _Stop().
                */
                virtual void _UpdateTp() = 0;

                void _Disable();
                Strategy::Strategy& _strategy;
                Controller::Status _status;
                float _open;
                float _lots;
                float _sl;
                float _tp;
                float _askRequote;
                float _bidRequote;
                bool _enabled;
                std::string _name;
                bool _logStartStop;
        };
    }
}

#endif
