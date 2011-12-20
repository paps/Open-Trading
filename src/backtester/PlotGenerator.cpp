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

#include <fstream>
#include "PlotGenerator.hpp"
#include "Conf.hpp"
#include "Logger.hpp"

#define CLASS "[Backtester/PlotGenerator] "

namespace Backtester
{
    PlotGenerator::PlotGenerator(Logger const& logger, Conf const& conf) :
        _logger(logger), _conf(conf)
    {
    }

    void PlotGenerator::WriteToDisk() const
    {
        std::ofstream f(this->_conf.plotDataFile.c_str());
        std::vector<PlotData>::const_iterator it = this->_values.begin();
        std::vector<PlotData>::const_iterator itEnd = this->_values.end();
        for (; it != itEnd; ++it)
            f << it->time << "\t" << it->balance << "\t" << it->equity << std::endl;
        this->_WriteSettings();
        if (f.good())
            this->_logger.Log(CLASS "Wrote plot data file \"" + this->_conf.plotDataFile + "\".");
        else
            this->_logger.Log(CLASS "Failed to write plot data file \"" + this->_conf.plotDataFile + "\".", ::Logger::Warning);
    }

    void PlotGenerator::_WriteSettings() const
    {
        std::ofstream f(this->_conf.plotSettingsFile.c_str());
        f << "set title \"Open Trading Backtest Result\"" << std::endl;
        f << "set key outside" << std::endl;
        f << "set grid" << std::endl;
        f << "set timefmt \"%s\"" << std::endl;
        f << "set xdata time" << std::endl;
        f << "set format x \"%a %d %b %H:%M\"" << std::endl;
        f << "set xlabel \"Time\"" << std::endl;
        f << "set ylabel \"" << this->_conf.counterCurrency << "\"" << std::endl;
        f << "plot \"" << this->_conf.plotDataFile << "\" using 1:3 title \"Equity\" with lines, \\" << std::endl;
        f << "\"\" using 1:2 title \"Balance\" with lines" << std::endl;
        if (f.good())
            this->_logger.Log(CLASS "Wrote plot settings file \"" + this->_conf.plotSettingsFile + "\".");
        else
            this->_logger.Log(CLASS "Failed to write plot settings file \"" + this->_conf.plotSettingsFile + "\".", ::Logger::Warning);
    }

    void PlotGenerator::ClearData()
    {
        this->_values.clear();
    }

    void PlotGenerator::AddData(time_t time, float balance, float equity)
    {
        PlotData v;
        v.time = time;
        v.balance = balance;
        v.equity = equity;
        this->_values.push_back(v);
    }
}
