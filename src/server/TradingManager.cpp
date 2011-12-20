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

#include "TradingManager.hpp"
#include "packet/Packet.hpp"
#include "core/StrategyInstantiator.hpp"
#include "core/StratParamsConf.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/signal/Signal.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "LogExporter.hpp"
#include "Feedback.hpp"
#include "conf/Conf.hpp"
#include "Cli.hpp"
#include "server/qt/Network.hpp"

#define CLASS "[Server/TradingManager] "

namespace Server
{
    TradingManager::TradingManager(Server& server) :
        _server(server), _controller(0), _paused(false), _tradingEnabled(false)
    {
        this->_logger = new Logger(this->_server.GetUi(), this->_server.GetConf().Read<bool>("coreLogToUi", true), this->_server.GetConf().Read<bool>("coreLogToStdOut", false));
        this->_logExporter = new LogExporter(*this->_logger);
        this->_feedback = new Feedback(this->_server.GetUi().IsGraphical());
        this->_stratParams = new Core::StratParamsConf(this->_server.GetConf().Read<std::string>("strategyParams", "strategy.lua"), *this->_logger);
        this->_strategyInstantiator = new Core::StrategyInstantiator(*this->_logger, *this->_feedback, *this->_stratParams);
        this->_logExporter->Start();
    }

    TradingManager::~TradingManager()
    {
        this->_DisableController();
        delete this->_strategyInstantiator;
        delete this->_stratParams;
        delete this->_feedback;
        delete this->_logExporter;
        delete this->_logger;
    }

    void TradingManager::Pause(bool pause /* = true */)
    {
        if (this->_paused && !pause && this->_continuePacket) // from paused to unpaused, with a discarded order packet
        {
            Packet::Packet p;
            p.WriteByte(Packet::Continue);
            this->_server.GetNetwork().Send(p);
            this->_logger->Log(CLASS "Continue packet sent. The client should stop the synchronous network wait.", ::Logger::Warning);
        }
        else if (this->_paused && !pause) // from paused to unpaused
            this->_logger->Log(CLASS "Server unpaused (no Continue packet needed).");
        else if (!this->_paused && pause) // from unpaused to paused
            this->_logger->Log(CLASS "Pausing server on next tick.");
        this->_paused = pause;
    }

    void TradingManager::EnableTrading(bool enabled /* = true */)
    {
        if (this->_tradingEnabled && !enabled) // from enabled to disabled
            this->_logger->Log(CLASS "Trading disabled.", ::Logger::Warning);
        else if (!this->_tradingEnabled && enabled) // from disabled to enabled
            this->_logger->Log(CLASS "Trading enabled.", ::Logger::Warning);
        this->_tradingEnabled = enabled;
    }

    void TradingManager::BeginProcessing()
    {
        this->_logger->BeginBuffering();
    }

    void TradingManager::EndProcessing()
    {
        this->_logger->EndBuffering();
        this->_server.GetUi().NewFeedback(*this->_feedback);
        this->_feedback->Wash();
    }

    bool TradingManager::ProcessPacket(Packet::Packet& p)
    {
        try
        {
            int type = p.ReadByte();
            switch (type)
            {
                case Packet::Tick:
                    return this->_ProcessTick(p);
                case Packet::Trade:
                    return this->_ProcessTrade(p);
                case Packet::Bar:
                    return this->_ProcessBar(p);
                case Packet::Hello:
                    return this->_ProcessHello(p);
                default:
                    return false;
            }
        }
        catch (std::exception& e)
        {
            this->_logger->Log(CLASS "Failed to process packet: \"" + QString(e.what()) + "\".", ::Logger::Error);
        }
        return true;
    }

    bool TradingManager::_ProcessTick(Packet::Packet& p)
    {
        // read packet
        float open = p.ReadFloat();
        float high = p.ReadFloat();
        float low = p.ReadFloat();
        float close = p.ReadFloat();
        float ask = p.ReadFloat();
        float bid = p.ReadFloat();
        Core::Controller::Status status = static_cast<Core::Controller::Status>(p.ReadInt());
        bool newBar = p.ReadByte();

        // log packet
        if (this->_server.GetNetwork().LogPackets())
            Cli::LogToStdOut(CLASS "Packet Tick:"
                    " open " + QString::number(open) +
                    " high " + QString::number(high) +
                    " low " + QString::number(low) +
                    " close " + QString::number(close) +
                    " ask " + QString::number(ask) +
                    " bid " + QString::number(bid) +
                    " status " + QString::number(status) + " \"" + QString::fromStdString(Core::Controller::ToString(status)) + "\""
                    " newBar " + (newBar ? "yes" : "no") +
                    ".");

        // process packet
        if (!this->_ControllerEnabled())
            return false;
        Core::Bar bar;
        bar.o = open;
        bar.h = high;
        bar.l = low;
        bar.c = close;
        bar.valid = true;
        this->_controller->ProcessTick(bar, ask, bid, status, newBar);

        // send response
        this->_SendTickResponse();
        return true;
    }

    bool TradingManager::_ProcessTrade(Packet::Packet& p)
    {
        // read packet
        Core::Controller::Status status = static_cast<Core::Controller::Status>(p.ReadInt());
        float open = p.ReadFloat();
        float lots = p.ReadFloat();
        float sl = p.ReadFloat();
        float tp = p.ReadFloat();
        float askRequote = p.ReadFloat();
        float bidRequote = p.ReadFloat();

        // log packet
        if (this->_server.GetNetwork().LogPackets())
            Cli::LogToStdOut(CLASS "Packet Trade:"
                    " status " + QString::number(status) + " \"" + QString::fromStdString(Core::Controller::ToString(status)) + "\""
                    " open " + QString::number(open) +
                    " lots " + QString::number(lots) +
                    " sl " + QString::number(sl) +
                    " tp " + QString::number(tp) +
                    " askRequote " + QString::number(askRequote) +
                    " bidRequote " + QString::number(bidRequote) +
                    ".");

        // process packet
        if (!this->_ControllerEnabled())
            return false;
        this->_controller->ProcessTrade(status, open, lots, sl, tp, askRequote, bidRequote);

        // no response
        return true;
    }

    bool TradingManager::_ProcessBar(Packet::Packet& p)
    {
        // read packet
        float open = p.ReadFloat();
        float high = p.ReadFloat();
        float low = p.ReadFloat();
        float close = p.ReadFloat();

        // log packet
        if (this->_server.GetNetwork().LogPackets())
            Cli::LogToStdOut(CLASS "Packet Bar:"
                    " open " + QString::number(open) +
                    " high " + QString::number(high) +
                    " low " + QString::number(low) +
                    " close " + QString::number(close) +
                    ".");

        // process packet
        if (!this->_ControllerEnabled())
            return false;
        Core::Bar bar;
        bar.o = open;
        bar.h = high;
        bar.l = low;
        bar.c = close;
        bar.valid = true;
        this->_controller->ProcessBar(bar);

        // no response
        return true;
    }

    bool TradingManager::_ProcessHello(Packet::Packet& p)
    {
        // read packet
        int versionMajor = p.ReadInt();
        int versionMinor = p.ReadInt();
        std::string pair = p.ReadString();
        unsigned int period = p.ReadInt();
        unsigned int digits = p.ReadInt();
        std::string password = p.ReadString();

        // log packet
        if (this->_server.GetNetwork().LogPackets())
            Cli::LogToStdOut(CLASS "Packet Hello:"
                    " versionMajor " + QString::number(versionMajor) +
                    " versionMinor " + QString::number(versionMajor) +
                    " pair \"" + QString::fromStdString(pair) + "\""
                    " period " + QString::number(period) +
                    " digits " + QString::number(digits) +
                    " password ***.");

        // process packet
        if (this->_ControllerEnabled())
        {
            this->_logger->Log(CLASS "Hello packet already received.", ::Logger::Error);
            return false;
        }
        if (versionMajor != ProtocolVersionMajor || versionMinor != ProtocolVersionMinor)
        {
            this->_logger->Log(CLASS "Bad protocol version (got "
                    + QString::number(versionMajor) + "." + QString::number(versionMinor) + ", expected "
                    + QString::number(ProtocolVersionMajor) + "." + QString::number(ProtocolVersionMinor) + ").", ::Logger::Error);
            return false;
        }
        if (this->_server.GetConf().Read<std::string>("password", "") != password)
        {
            this->_logger->Log(CLASS "Bad password.", ::Logger::Error);
            return false;
        }
        if (!this->_EnableController(pair, period, digits))
            return false;

        // send response
        Packet::Packet welcome;
        welcome.WriteByte(Packet::Welcome);
        welcome.WriteInt(this->_strategyInstantiator->GetStrategy()->GetSignal().GetMinBars());
        this->_server.GetNetwork().Send(welcome);
        this->_logger->Log(CLASS "Requesting " + QString::number(this->_strategyInstantiator->GetStrategy()->GetSignal().GetMinBars()) + " bars.");
        return true;
    }

    void TradingManager::_SendTickResponse()
    {
        if (this->_paused)
        {
            this->_logger->Log(CLASS "Server paused.");
            this->_logger->Log(CLASS "Order packet not sent. Client is in synchronous network wait.", ::Logger::Warning);
            this->_continuePacket = true;
            return;
        }
        Core::Controller::Output const& s = this->_controller->GetLastOutput();
        Packet::Packet p;
        p.WriteByte(Packet::Order);
        if (this->_closePosition)
        {
            p.WriteInt(Core::Controller::OrderClose);
            this->_logger->Log(CLASS "Order packet \"" + std::string(Core::Controller::ToString(s.order)) + "\" hijacked: closing position.", ::Logger::Warning);
        }
        else if (this->_tradingEnabled || s.order == Core::Controller::OrderClose || s.order == Core::Controller::OrderAdjust)
        {
            p.WriteInt(s.order);
            if (s.order != Core::Controller::OrderNothing)
                this->_logger->Log(CLASS "Sending order \"" + std::string(Core::Controller::ToString(s.order)) + "\".");
        }
        else
        {
            if (s.order != Core::Controller::OrderNothing)
                this->_logger->Log(CLASS "Order packet \"" + std::string(Core::Controller::ToString(s.order)) + "\" hijacked: trading is disabled.", ::Logger::Warning);
            p.WriteInt(Core::Controller::OrderNothing);
        }
        p.WriteFloat(s.lots);
        p.WriteFloat(s.sl);
        p.WriteFloat(s.tp);
        this->_server.GetNetwork().Send(p);
        this->_closePosition = false;
    }

    void TradingManager::ClosePosition()
    {
        this->_closePosition = true;
        this->_logger->Log(CLASS "Closing position on next tick (Tick packet response hijack).");
    }

    void TradingManager::ClientConnected(QString const& name)
    {
        this->_server.GetUi().ClientConnected(name);
    }

    void TradingManager::ClientDisconnected()
    {
        this->_DisableController();
        this->_server.GetUi().NewFeedback(*this->_feedback);
        this->_server.GetUi().ClientDisconnected();
    }

    bool TradingManager::_EnableController(std::string const& pair, unsigned int period, unsigned int digits)
    {
        this->_closePosition = false;
        this->_continuePacket = false;
        this->_logger->Log(CLASS "Creating controller...");
        if (this->_strategyInstantiator->Instantiate(this->_server.GetConf().Read<std::string>("strategy", ""), pair, period, digits))
        {
            this->_controller = new Core::Controller(*this->_strategyInstantiator->GetStrategy());
            this->_logger->Log(CLASS "Controller successfully created.");
            return true;
        }
        this->_logger->Log(CLASS "Failed to create controller.", ::Logger::Warning);
        return false;
    }

    void TradingManager::_DisableController()
    {
        if (this->_ControllerEnabled())
            this->_logger->Log(CLASS "Destroying controller...");
        delete this->_controller;
        this->_controller = 0;
        if (this->_strategyInstantiator->StrategyInstantiated())
            this->_strategyInstantiator->Destroy();
    }

    bool TradingManager::_ControllerEnabled()
    {
        return this->_strategyInstantiator->StrategyInstantiated() || this->_controller;
    }
}
