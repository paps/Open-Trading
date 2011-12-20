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

#include "Conf.hpp"
#include "conf/Conf.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Backtester/Conf] "

namespace Backtester
{
    Conf::Conf(::Conf::Conf& from, Logger const& logger)
    {
        this->strategy = from.Read<std::string>("strategy", "");
        this->strategyParams = from.Read<std::string>("strategyParams", "");
        this->pair = from.Read<std::string>("pair", "");
        if (this->pair.size() != 6)
        {
            logger.Log(CLASS "Pair \"" + this->pair + "\" seems invalid.", ::Logger::Warning);
            this->baseCurrency = "_BASE_CUR_";
            this->counterCurrency = "_COUNTER_CUR_";
        }
        else
        {
            this->baseCurrency = this->pair.substr(0, 3);
            this->counterCurrency = this->pair.substr(3, 3);
        }
        this->period = from.Read<unsigned int>("period", 1);
        this->digits = from.Read<unsigned int>("digits", 5);
        if (this->digits > 5)
        {
            logger.Log(CLASS "Invalid number of digits (" + Tools::ToString(this->digits) + "), changing to " + Tools::ToString(5) + ".", ::Logger::Warning);
            this->digits = 5;
        }
        this->spread = from.Read<float>("spread", 1.8);
        if (this->spread < 0.5 || this->spread > 10)
        {
            logger.Log(CLASS "Invalid spread of " + Tools::ToString(this->spread, 2) + ", changing to " + Tools::ToString(1.8, 2) + ".", ::Logger::Warning);
            this->spread = 1.8;
        }
        this->minPriceOffset = from.Read<float>("minPriceOffset", 5);
        if (this->minPriceOffset < 1 || this->minPriceOffset > 100)
        {
            logger.Log(CLASS "Invalid minimal price offset of " + Tools::ToString(this->minPriceOffset, 2) + ", changing to " + Tools::ToString(5, 2) + ".", ::Logger::Warning);
            this->minPriceOffset = 5;
        }
        this->optimizationMode = from.Read<bool>("optimizationMode", false);
        this->paramsGenerator = from.Read<std::string>("paramsGenerator", "complete");
        if (this->optimizationMode)
        {
            this->threads = from.Read<unsigned int>("threads", 3);
            if (this->threads < 1 || this->threads > 20)
            {
                logger.Log(CLASS "Invalid thread number of " + Tools::ToString(this->threads) + ", changing to " + Tools::ToString(3) + ".", ::Logger::Warning);
                this->threads = 3;
            }
        }
        else
            this->threads = 1;
        this->confirmLaunch = from.Read<bool>("confirmLaunch", true);
        this->showTradeActions = from.Read<bool>("showTradeActions", true);
        this->showTradeDetails = from.Read<bool>("showTradeDetails", true);
        this->deposit = from.Read<float>("deposit", 10000);
        this->plotOutput = from.Read<bool>("plotOutput", false);
        this->plotDataFile = from.Read<std::string>("plotDataFile", "backtest.dat");
        this->plotSettingsFile = from.Read<std::string>("plotSettingsFile", "backtest.plot");
        this->resultRanking = from.Read<std::string>("resultRanking", "profit");
        this->fewerTicks = from.Read<bool>("fewerTicks", false);
        this->_Dump(logger);
    }

    void Conf::_Dump(Logger const& logger)
    {
        logger.Log(CLASS "Configuration dump:");
        logger.Log(CLASS "  - strategy: \"" + this->strategy + "\"");
        logger.Log(CLASS "  - strategyParams: \"" + this->strategyParams + "\"");
        logger.Log(CLASS "  - pair: \"" + this->pair + "\"");
        logger.Log(CLASS "  - baseCurrency: \"" + this->baseCurrency + "\"");
        logger.Log(CLASS "  - counterCurrency: \"" + this->counterCurrency + "\"");
        logger.Log(CLASS "  - period: " + Tools::ToString(this->period));
        logger.Log(CLASS "  - digits: " + Tools::ToString(this->digits));
        logger.Log(CLASS "  - spread: " + Tools::ToString(this->spread, 1));
        logger.Log(CLASS "  - minPriceOffset: " + Tools::ToString(this->minPriceOffset, 1));
        logger.Log(CLASS "  - deposit: " + this->counterCurrency + " " + Tools::ToString(this->deposit, 2));
        logger.Log(CLASS "  - plotOutput: " + std::string(this->plotOutput ? "yes" : "no"));
        if (this->plotOutput)
        {
            logger.Log(CLASS "  - plotDataFile: \"" + this->plotDataFile + "\"");
            logger.Log(CLASS "  - plotSettingsFile: \"" + this->plotSettingsFile + "\"");
        }
    }
}
