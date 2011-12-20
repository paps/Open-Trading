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

#include "Server.hpp"
#include "TradingManager.hpp"
#include "server/qt/Network.hpp"
#include "conf/Conf.hpp"
#include "Cli.hpp"
#include "Gui.hpp"

#define CLASS "[Server/Server] "

namespace Server
{
    Server::Server(int& ac, char** av)
    {
        this->_conf = new Conf::Conf("server.lua");
        if (this->_conf->Read<bool>("graphicalUi", false))
        {
            this->_app = new QApplication(ac, av);
            this->_ui = new Gui(*this);
        }
        else
        {
            this->_app = new QCoreApplication(ac, av);
            this->_ui = new Cli(*this);
        }
        this->_tradingManager = new TradingManager(*this);
        this->_network = new Qt::Network(*this);
        this->_tradingManager->EnableTrading(this->_conf->Read<bool>("enableTrading", false));
        this->_ui->Log(CLASS "* Welcome to the Open Trading Server *");
        if (this->_conf->Error())
            this->_ui->Log(CLASS "Failed to load configuration file: \"" + this->_conf->GetLastError() + "\".", ::Logger::Error);
    }

    Server::~Server()
    {
        delete this->_network;
        delete this->_tradingManager;
        delete this->_ui;
        delete this->_app;
        delete this->_conf;
    }

    int Server::Run()
    {
        return this->_app->exec();
    }

    Conf::Conf& Server::GetConf()
    {
        return *this->_conf;
    }

    Ui& Server::GetUi()
    {
        return *this->_ui;
    }

    Qt::Network& Server::GetNetwork()
    {
        return *this->_network;
    }

    TradingManager& Server::GetTradingManager()
    {
        return *this->_tradingManager;
    }
}
