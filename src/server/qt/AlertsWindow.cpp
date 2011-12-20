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
#include "AlertsWindow.hpp"
#include "MainWindow.hpp"
#include "server/Gui.hpp"
#include "server/SoundManager.hpp"

namespace Server
{
    namespace Qt
    {
        AlertsWindow::AlertsWindow(Gui& gui) :
            QMainWindow(&gui.GetMainWindow(), ::Qt::Tool),
            _gui(gui),
            _activeAlerts(0),
            _oldAlerts(0),
            _alternatingColor(true)
        {
            this->setupUi(this);
            this->_gui.GetSettings().beginGroup("AlertsWindow");
            if (this->_gui.GetSettings().contains("geometry"))
                this->restoreGeometry(this->_gui.GetSettings().value("geometry").toByteArray());
            this->_gui.GetSettings().endGroup();
            this->_UpdateLabels();
        }

        void AlertsWindow::Log(QString const& msg, ::Logger::MessageType type)
        {
            ++this->_activeAlerts;
            ++this->_oldAlerts;
            this->_UpdateLabels();
            QString log = "<b>" + QTime::currentTime().toString("hh:mm:ss") + " " + (type == ::Logger::Warning ? "W" : "E") + "</b> " + msg;
            this->alertsPlainTextEdit->appendHtml(log);
            this->logPlainTextEdit->appendHtml(log);
            if (type == ::Logger::Warning)
                this->_gui.GetSoundManager().PlaySound(SoundManager::SoundWarning);
            else
                this->_gui.GetSoundManager().PlaySound(SoundManager::SoundError);
        }

        void AlertsWindow::SecondTick()
        {
            if (!this->_activeAlerts)
                return;
            if (this->_alternatingColor)
                this->activeAlertsLabel->setStyleSheet("QLabel { background-color: red; }");
            else
                this->activeAlertsLabel->setStyleSheet("QLabel { background-color: none; }");
            this->_alternatingColor = !this->_alternatingColor;
        }

        void AlertsWindow::on_clearPushButton_clicked()
        {
            this->_activeAlerts = 0;
            this->_UpdateLabels();
            this->alertsPlainTextEdit->clear();
            this->activeAlertsLabel->setStyleSheet("QLabel { background-color: none; }");
        }

        void AlertsWindow::_UpdateLabels()
        {
            if (this->_activeAlerts == 0)
                this->activeAlertsLabel->setText("No active alerts");
            else
                this->activeAlertsLabel->setText(QString::number(this->_activeAlerts) + " active alert" + (this->_activeAlerts > 1 ? "s" : ""));
            if (this->_oldAlerts == 0)
                this->oldAlertsLabel->setText("No logged alerts");
            else
                this->oldAlertsLabel->setText(QString::number(this->_oldAlerts) + " logged alert" + (this->_oldAlerts > 1 ? "s" : ""));
        }

        void AlertsWindow::closeEvent(QCloseEvent*)
        {
            this->_gui.GetSettings().beginGroup("AlertsWindow");
            this->_gui.GetSettings().setValue("geometry", this->saveGeometry());
            this->_gui.GetSettings().endGroup();
        }
    }
}
