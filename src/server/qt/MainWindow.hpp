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

#ifndef __SERVER_QT_MAINWINDOW__
#define __SERVER_QT_MAINWINDOW__

#include <QTimer>
#include "ui_main.h"
#include "logger/Logger.hpp"
#include "server/TradingManager.hpp"

namespace Server
{
    class Gui;

    namespace Qt
    {
        class MainWindow :
            public QMainWindow, private ::Ui::Main
        {
            Q_OBJECT
            public:
                explicit MainWindow(Gui& gui);
                void ClientConnected(QString const& name);
                void ClientDisconnected();
                void Log(QString const& msg, ::Logger::MessageType type);
                void SecondTick();
                void NewFeedback(Feedback const& feedback);
            private slots:
                void on_actionQuit_triggered();
                void on_actionAlerts_triggered();
                void on_actionKickClient_triggered();
                void on_closePositionPushButton_clicked();
                void on_pauseCheckBox_stateChanged(int state);
                void on_enableTradingCheckBox_stateChanged(int state);
                void _SecondTickerFired();
            private:
                virtual void closeEvent(QCloseEvent*);
                Gui& _gui;
                QTimer _secondTicker;
                unsigned int _updates;
        };
    }
}

#endif
