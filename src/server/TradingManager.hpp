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

#ifndef __SERVER_TRADINGMANAGER__
#define __SERVER_TRADINGMANAGER__

#include <boost/noncopyable.hpp>
#include <QString>
#include "core/Controller.hpp"

namespace Packet
{
    class Packet;
}
namespace Core
{
    class StrategyInstantiator;
    class StratParamsConf;
    namespace Strategy
    {
        class Strategy;
    }
}

namespace Server
{
    class Server;
    class Logger;
    class Feedback;
    class LogExporter;

    class TradingManager :
        private boost::noncopyable
    {
        public:
            explicit TradingManager(Server& server);
            ~TradingManager();
            void BeginProcessing();
            void EndProcessing();
            bool ProcessPacket(Packet::Packet& p);
            void ClientConnected(QString const& name);
            void ClientDisconnected();
            void ClosePosition();
            void Pause(bool pause = true);
            void EnableTrading(bool enable = true);
        private:
            enum
            {
                ProtocolVersionMajor = 1,
                ProtocolVersionMinor = 1,
            };
            bool _ProcessTick(Packet::Packet& p);
            bool _ProcessTrade(Packet::Packet& p);
            bool _ProcessBar(Packet::Packet& p);
            bool _ProcessHello(Packet::Packet& p);
            bool _EnableController(std::string const& pair, unsigned int period, unsigned int digits);
            void _DisableController();
            bool _ControllerEnabled();
            void _SendTickResponse();
            Server& _server;
            Core::StrategyInstantiator* _strategyInstantiator;
            Core::Controller* _controller;
            Logger* _logger;
            Feedback* _feedback;
            Core::StratParamsConf* _stratParams;
            LogExporter* _logExporter;
            bool _closePosition;
            bool _continuePacket;
            bool _paused;
            bool _tradingEnabled;
    };
}

#endif
