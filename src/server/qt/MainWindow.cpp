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

#include <QTime>
#include "MainWindow.hpp"
#include "AlertsWindow.hpp"
#include "server/Gui.hpp"
#include "server/Server.hpp"
#include "Network.hpp"
#include "conf/Conf.hpp"
#include "server/QSignalBlocker.hpp"
#include "server/Feedback.hpp"

namespace
{
    template <typename T>
        QString SanitizeVal(T val)
        {
            if (val <= 0)
                return "-";
            return QString::number(val);
        }

    template <>
        QString SanitizeVal(unsigned int val)
        {
            if (val == 0 || val == UINT32_MAX)
                return "-";
            return QString::number(val);
        }
}

namespace Server
{
    namespace Qt
    {
        MainWindow::MainWindow(Gui& gui) :
            _gui(gui), _updates(0)
        {
            this->setupUi(this);
            this->_gui.GetSettings().beginGroup("MainWindow");
            if (this->_gui.GetSettings().contains("geometry"))
                this->restoreGeometry(this->_gui.GetSettings().value("geometry").toByteArray());
            this->_gui.GetSettings().endGroup();
            if (this->_gui.GetServer().GetConf().Read<bool>("enableTrading", false))
            {
                QSignalBlocker b(this->enableTradingCheckBox);
                this->enableTradingCheckBox->setChecked(true);
            }
            this->connect(&this->_secondTicker, SIGNAL(timeout()), this, SLOT(_SecondTickerFired()));
            this->_secondTicker.start(1000);
        }

        void MainWindow::NewFeedback(Feedback const& feedback)
        {
            // market group
            Core::Feedback::State::Market const& market = feedback.GetMarketInfo();
            if (market.dirty)
            {
                this->marketAskLabel->setText(SanitizeVal(market.ask));
                this->marketBidLabel->setText(SanitizeVal(market.bid));
                this->marketSpdLabel->setText(market.spd <= 0 ? "-" : QString::number(market.spd, 'g', 2));
                this->marketOLabel->setText(SanitizeVal(market.bar.o));
                this->marketHLabel->setText(SanitizeVal(market.bar.h));
                this->marketCLabel->setText(SanitizeVal(market.bar.c));
                this->marketLLabel->setText(SanitizeVal(market.bar.l));
                this->barsTicksLabel->setText(SanitizeVal(market.ticks)); // :o
            }
            // bars group
            Core::Feedback::State::Bars const& bars = feedback.GetBarsInfo();
            if (bars.dirty)
            {
                this->barsPairLabel->setText(QString::fromStdString(bars.pair));
                this->barsPeriodLabel->setText(SanitizeVal(bars.period));
                this->barsDigitsLabel->setText(SanitizeVal(bars.digits));
            }
            // position group
            Core::Feedback::State::Position const& position = feedback.GetPositionInfo();
            if (position.dirty)
            {
                this->positionStatusLabel->setText(Core::Controller::ToString(position.status));
                this->positionPipsLabel->setText(position.open <= 0 ? "-" : QString::number(position.pips, 'g', 2));
                this->positionLotsLabel->setText(SanitizeVal(position.lots));
                this->positionOpenLabel->setText(SanitizeVal(position.open));
                this->positionSlLabel->setText(SanitizeVal(position.sl));
                this->positionTpLabel->setText(SanitizeVal(position.tp));
            }
            // client group
            this->clientUpdatesLabel->setText(QString::number(++this->_updates));
        }

        void MainWindow::ClientConnected(QString const& name)
        {
            this->clientStatusLabel->setText("Connected");
            this->clientNameLabel->setText(name);
            this->marketGroupBox->setEnabled(true);
            this->barsGroupBox->setEnabled(true);
            this->positionGroupBox->setEnabled(true);
        }

        void MainWindow::ClientDisconnected()
        {
            this->clientStatusLabel->setText("Disconnected");
            this->clientNameLabel->setText("-");
            this->marketGroupBox->setEnabled(false);
            this->barsGroupBox->setEnabled(false);
            this->positionGroupBox->setEnabled(false);
            this->_updates = 0;
            this->clientUpdatesLabel->setText(QString::number(this->_updates));
        }

        void MainWindow::Log(QString const& msg, ::Logger::MessageType type)
        {
            char prefix;
            switch (type)
            {
                case ::Logger::Warning:
                    prefix = 'W';
                    break;
                case ::Logger::Error:
                    prefix = 'E';
                    break;
                default:
                    prefix = 'I';
            }
            this->logPlainTextEdit->appendHtml("<b>" + QTime::currentTime().toString("hh:mm:ss") + " " + prefix + "</b> " + msg);
        }

        void MainWindow::SecondTick()
        {
        }

        void MainWindow::_SecondTickerFired()
        {
            this->SecondTick();
            this->_gui.GetAlertsWindow().SecondTick();
        }

        void MainWindow::on_actionQuit_triggered()
        {
            this->close();
        }

        void MainWindow::on_actionAlerts_triggered()
        {
            this->_gui.GetAlertsWindow().show();
        }

        void MainWindow::on_actionKickClient_triggered()
        {
            this->_gui.GetServer().GetNetwork().Disconnect();
        }

        void MainWindow::on_closePositionPushButton_clicked()
        {
            this->_gui.GetServer().GetTradingManager().ClosePosition();
        }

        void MainWindow::on_pauseCheckBox_stateChanged(int state)
        {
            this->_gui.GetServer().GetTradingManager().Pause(state == ::Qt::Checked);
        }

        void MainWindow::on_enableTradingCheckBox_stateChanged(int state)
        {
            this->_gui.GetServer().GetTradingManager().EnableTrading(state == ::Qt::Checked);
        }

        void MainWindow::closeEvent(QCloseEvent*)
        {
            this->_gui.GetSettings().beginGroup("MainWindow");
            this->_gui.GetSettings().setValue("geometry", this->saveGeometry());
            this->_gui.GetSettings().endGroup();
            if (this->_gui.GetAlertsWindow().isVisible())
                this->_gui.GetAlertsWindow().close();
        }
    }
}
