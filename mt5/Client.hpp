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

#ifndef __CLIENT__
#define __CLIENT__

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "../../packet/Packet.hpp"
#include "MetaTrader.hpp"

class Client
{
	public:
		explicit Client(std::string const& host,
                std::string const& port,
                std::string const& historyFile,
                unsigned int periodSeconds,
                unsigned int digits,
                std::string const& password,
                MetaTrader::Server& server);
		~Client();
		bool Tick(MetaTrader::Input& in, MetaTrader::Output& out); // true when Continue, false when Order, throw otherwise
		void AddBar(float open, float high, float low, float close);
		void TradeInfo(MetaTrader::Trade& trade);
	private:
		enum
		{
			ProtocolVersionMajor = 1,
			ProtocolVersionMinor = 1,
		};
		void _SendPacket(Packet::Packet const& packet);
        void _ReceivePacket(Packet::Packet& packet);
		boost::asio::io_service _ioService;
		boost::asio::ip::tcp::socket _socket;
};

#endif
