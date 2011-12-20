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

#include "Gui.hpp"
#include "server/qt/MainWindow.hpp"
#include "server/qt/AlertsWindow.hpp"
#include "server/SoundManager.hpp"

#define CLASS "[Server/Gui] "

namespace Server
{
    Gui::Gui(Server& server) :
        Ui(server), _settings("server-gui.ini", QSettings::IniFormat)
    {
        this->_mainWindow = new Qt::MainWindow(*this);
        this->_mainWindow->show();
        this->_alertsWindow = new Qt::AlertsWindow(*this);
        this->_soundManager = new SoundManager(*this);
        this->Log(CLASS "Using graphical interface.");
        this->ClientDisconnected();
    }

    Gui::~Gui()
    {
        delete this->_soundManager;
        delete this->_alertsWindow;
        delete this->_mainWindow;
    }

    SoundManager& Gui::GetSoundManager()
    {
        return *this->_soundManager;
    }

    bool Gui::IsGraphical() const
    {
        return true;
    }

    void Gui::ClientConnected(QString const& name)
    {
        this->_mainWindow->ClientConnected(name);
        this->Log(CLASS "Client " + name + " connected.", ::Logger::Warning);
    }

    void Gui::ClientDisconnected()
    {
        this->_mainWindow->ClientDisconnected();
        this->Log(CLASS "Client disconnected.", ::Logger::Warning);
    }

    void Gui::NewFeedback(Feedback const& feedback)
    {
        this->_mainWindow->NewFeedback(feedback);
    }

    void Gui::Log(QString const& msg, ::Logger::MessageType type /* = ::Logger::Info */)
    {
        this->_mainWindow->Log(msg, type);
        if (type != ::Logger::Info)
        {
            this->_alertsWindow->Log(msg, type);
            this->_alertsWindow->show();
        }
    }

    Qt::MainWindow& Gui::GetMainWindow()
    {
        return *this->_mainWindow;
    }

    Qt::AlertsWindow& Gui::GetAlertsWindow()
    {
        return *this->_alertsWindow;
    }

    QSettings& Gui::GetSettings()
    {
        return this->_settings;
    }
}
