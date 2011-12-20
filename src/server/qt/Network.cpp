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

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "Network.hpp"
#include "server/Server.hpp"
#include "packet/Packet.hpp"
#include "server/TradingManager.hpp"
#include "conf/Conf.hpp"
#include "server/Cli.hpp"

#define CLASS "[Server/Qt/Network] "

namespace Server
{
    namespace Qt
    {
        Network::Network(Server& server) :
            _server(server), _socket(0), _packetSize(0)
        {
            this->_logPackets = this->_server.GetConf().Read<bool>("logPackets", true);
            this->_buffer = new char[USHRT_MAX];
            this->_listener = new QTcpServer;
            this->_listener->setMaxPendingConnections(1);
            this->connect(this->_listener, SIGNAL(newConnection()), this, SLOT(_SocketConnected()));
            QString host = QString::fromStdString(this->_server.GetConf().Read<std::string>("host", "127.0.0.1"));
            quint16 port = this->_server.GetConf().Read<quint16>("port", 42042);
            if (!this->_listener->listen(QHostAddress(host), port))
            {
                this->_server.GetUi().Log(CLASS "Failed to start listening on " + host + ":" + QString::number(port) + ".", ::Logger::Error);
                this->_server.GetUi().Log(CLASS "QTcpServer says: \"" + this->_listener->errorString() + "\".", ::Logger::Error);
            }
            else
                this->_server.GetUi().Log(CLASS "Listening on " + this->_listener->serverAddress().toString() + ":" + QString::number(this->_listener->serverPort()) + ".");
        }

        Network::~Network()
        {
            delete this->_listener;
            delete [] this->_buffer;
        }

        bool Network::LogPackets() const
        {
            return this->_logPackets;
        }

        void Network::Send(Packet::Packet const& p)
        {
            if (!this->_socket || this->_socket->state() != QAbstractSocket::ConnectedState)
                return;
            QByteArray bytes;
            QDataStream out(&bytes, QIODevice::WriteOnly);
            out.writeRawData(p.GetCompleteData(), p.GetCompleteSize());
            this->_socket->write(bytes);
            this->_socket->flush();
            if (this->_logPackets)
                Cli::LogToStdOut(CLASS "Packet of " + QString::number(p.GetCompleteSize()) + " bytes sent (" + QString::number(p.GetSize()) + " byte(s) of data).");
        }

        void Network::Disconnect()
        {
            if (!this->_socket)
                return;
            this->_socket->disconnectFromHost();
        }

        void Network::_SocketConnected()
        {
            QTcpSocket* socket = this->_listener->nextPendingConnection();
            if (!socket)
                return;
            if (this->_socket)
            {
                this->_server.GetUi().Log(CLASS "Rejecting new incoming connection from " + socket->peerAddress().toString() + ".", ::Logger::Warning);
                socket->deleteLater();
                return;
            }
            this->_socket = socket;
            this->connect(this->_socket, SIGNAL(readyRead()), this, SLOT(_SocketData()));
            this->connect(this->_socket, SIGNAL(disconnected()), this, SLOT(_SocketDisconnected()));
            this->_server.GetTradingManager().ClientConnected(this->_socket->peerAddress().toString() + ":" + QString::number(this->_socket->peerPort()));
        }

        void Network::_SocketDisconnected()
        {
            this->_socket->deleteLater();
            this->_socket = 0;
            this->_packetSize = 0;
            this->_server.GetTradingManager().ClientDisconnected();
        }

        void Network::_SocketData()
        {
            if (!this->_socket)
                return;
            this->_server.GetTradingManager().BeginProcessing();
            while (this->_socket->bytesAvailable())
            {
                QDataStream stream(this->_socket);
                if (!this->_packetSize)
                {
                    if (this->_socket->bytesAvailable() < sizeof(this->_packetSize))
                        break;
                    stream.readRawData(reinterpret_cast<char*>(&this->_packetSize), sizeof(this->_packetSize));
                    this->_packetSize = ntohs(this->_packetSize);
                }
                if (this->_socket->bytesAvailable() < this->_packetSize)
                    break;
                stream.readRawData(this->_buffer, this->_packetSize);
                Packet::Packet p;
                p.WriteData(this->_buffer, this->_packetSize);
                if (!this->_server.GetTradingManager().ProcessPacket(p))
                {
                    this->_socket->disconnectFromHost();
                    break;
                }
                this->_packetSize = 0;
            }
            this->_server.GetTradingManager().EndProcessing();
        }
    }
}
