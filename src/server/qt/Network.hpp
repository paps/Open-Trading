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

#ifndef __SEVER_QT_NETWORK__
#define __SEVER_QT_NETWORK__

#include <QTcpSocket>
#include <QTcpServer>
#include <boost/noncopyable.hpp>

namespace Packet
{
    class Packet;
}

namespace Server
{
    class Server;

    namespace Qt
    {
        class Network :
            public QObject, private boost::noncopyable
        {
            Q_OBJECT
            public:
                explicit Network(Server& server);
                ~Network();
                void Send(Packet::Packet const& p);
                void Disconnect();
                bool LogPackets() const;
            private slots:
                void _SocketConnected();
                void _SocketDisconnected();
                void _SocketData();
            private:
                Server& _server;
                QTcpServer* _listener;
                QTcpSocket* _socket;
                quint16 _packetSize;
                char* _buffer;
                bool _logPackets;
        };
    }
}

#endif
