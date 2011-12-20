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

#include "Client.hpp"

Client::Client(std::string const& host,
        std::string const& port,
        std::string const& historyFile,
        unsigned int periodSeconds,
        unsigned int digits,
        std::string const& password,
        MetaTrader::Server& server) :
	_socket(_ioService)
{
	boost::asio::ip::tcp::resolver resolver(this->_ioService);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host, port);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	this->_socket.connect(*iterator);

	Packet::Packet hello;
	hello.WriteByte(Packet::Hello);
    hello.WriteInt(ProtocolVersionMajor);
    hello.WriteInt(ProtocolVersionMinor);
	hello.WriteString(historyFile);
	hello.WriteInt(periodSeconds / 60);
	hello.WriteInt(digits);
    hello.WriteString(password);
	this->_SendPacket(hello);

    Packet::Packet welcome;
    this->_ReceivePacket(welcome);
    int type = welcome.ReadByte();
    if (type == Packet::Welcome)
    {
        server.neededBars = welcome.ReadInt();
    }
    else
        throw std::runtime_error("Did not receive a Welcome packet :(");
}

Client::~Client()
{
}

void Client::TradeInfo(MetaTrader::Trade& trade)
{
	Packet::Packet p;
	p.WriteByte(Packet::Trade);
	p.WriteInt(trade.status);
	p.WriteFloat(trade.open);
	p.WriteFloat(trade.lots);
	p.WriteFloat(trade.sl);
	p.WriteFloat(trade.tp);
    p.WriteFloat(trade.askRequote);
    p.WriteFloat(trade.bidRequote);
	this->_SendPacket(p);
}

void Client::AddBar(float open, float high, float low, float close)
{
	Packet::Packet p;
	p.WriteByte(Packet::Bar);
	p.WriteFloat(open);
	p.WriteFloat(high);
	p.WriteFloat(low);
	p.WriteFloat(close);
	this->_SendPacket(p);
}

bool Client::Tick(MetaTrader::Input& in, MetaTrader::Output& out)
{
	if (in.error == 0)
	{
		// send
		{
			Packet::Packet p;
			p.WriteByte(Packet::Tick);
			p.WriteFloat(in.open);
			p.WriteFloat(in.high);
			p.WriteFloat(in.low);
			p.WriteFloat(in.close);
			p.WriteFloat(in.ask);
			p.WriteFloat(in.bid);
			p.WriteInt(in.status);
			p.WriteByte(in.newBar);
			this->_SendPacket(p);
		}

		// receive
        Packet::Packet p;
        this->_ReceivePacket(p);

        // set output
		int type = p.ReadByte();
		if (type == Packet::Order)
		{
			out.order = p.ReadInt();
			out.lots = p.ReadFloat();
			out.sl = p.ReadFloat();
			out.tp = p.ReadFloat();
		}
		else if (type == Packet::Continue)
			return true;
        else
            throw std::runtime_error("Did not receive an Order packet :(");
	}
	return false;
}

void Client::_SendPacket(Packet::Packet const& packet)
{
	boost::asio::write(this->_socket, boost::asio::buffer(packet.GetCompleteData(), packet.GetCompleteSize()));
}

void Client::_ReceivePacket(Packet::Packet& packet)
{
    uint16_t packetSize;
    boost::asio::read(this->_socket, boost::asio::buffer(reinterpret_cast<char*>(&packetSize), 2));
    packetSize = ntohs(packetSize);
    char* packetData = new char[packetSize];
    boost::asio::read(this->_socket, boost::asio::buffer(packetData, packetSize));
    packet.WriteData(packetData, packetSize);
    delete [] packetData;
}
