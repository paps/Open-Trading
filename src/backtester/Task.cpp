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

#include "Task.hpp"
#include "TickGenerator.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"
#include "StratParamsMap.hpp"
#include "core/StrategyInstantiator.hpp"
#include "core/Controller.hpp"
#include "core/strategy/Strategy.hpp"
#include "core/actor/Actor.hpp"
#include "Feedback.hpp"
#include "Conf.hpp"
#include "Report.hpp"
#include "PlotGenerator.hpp"

#define CLASS "[Backtester/Task] "

namespace Backtester
{
    Task::Task(TickGenerator& tickGenerator,
            Logger const& logger,
            Conf const& conf,
            StratParamsMap& stratParams,
            Report& report) :
        _tickGenerator(tickGenerator),
        _logger(logger),
        _conf(conf),
        _stratParams(stratParams),
        _report(report),
        _plotGenerator(0)
    {
        this->_feedback = new Feedback();
        this->_strategyInstantiator = new Core::StrategyInstantiator(this->_logger, *this->_feedback, this->_stratParams);
        if (!this->_conf.optimizationMode && this->_conf.plotOutput)
            this->_plotGenerator = new PlotGenerator(this->_logger, this->_conf);
    }

    Task::~Task()
    {
        delete this->_plotGenerator;
        delete this->_strategyInstantiator;
        delete this->_feedback;
    }

    void Task::_ResetState()
    {
        this->_state.status = Core::Controller::StatusNothing;
        this->_state.open = -1;
        this->_state.lots = -1;
        this->_state.tp = -1;
        this->_state.sl = -1;
    }

    bool Task::Run()
    {
        this->_state.balance = this->_conf.deposit;
        this->_ResetState();
        this->_strategyInstantiator->Instantiate(this->_conf.strategy, this->_conf.pair, this->_conf.period, this->_conf.digits);
        if (!this->_strategyInstantiator->StrategyInstantiated())
        {
            this->_logger.Log(CLASS "Failed to instantiate strategy \"" + this->_conf.strategy + "\".", ::Logger::Error);
            return false;
        }
        this->_strategyInstantiator->GetStrategy()->GetActor().SetLogStartStop(false);
        Core::Controller* controller = new Core::Controller(*this->_strategyInstantiator->GetStrategy());
        this->_Run(*controller);
        delete controller;
        this->_strategyInstantiator->Destroy();
        return true;
    }

    void Task::_Run(Core::Controller& controller)
    {
        std::pair<float, float> tick; // tick.first -> ask, tick.second -> bid
        Core::Bar bar;
        TickGenerator::GenerationResult tickGen;
        while (true)
        {
            tickGen = this->_tickGenerator.GenerateNextTick(*this->_strategyInstantiator->GetStrategy(), tick, bar);
            if (tickGen == TickGenerator::Interruption)
            {
                controller.Interrupt();
                this->_Interrupt(bar, tick); // tick is the last tick unmodified by the last call to GenerateNextTick(), as is bar
                continue;
            }
            else if (tickGen == TickGenerator::NoMoreTicks)
                break;
            this->_PreTick(bar, tick);
            controller.ProcessTick(bar, tick.first, tick.second, this->_state.status, tickGen == TickGenerator::NewBarTick);
            if (this->_PostTick(bar, tick, controller.GetLastOutput()))
                controller.ProcessTrade(this->_state.status,
                        this->_state.open,
                        this->_state.lots,
                        this->_state.sl,
                        this->_state.tp,
                        tick.first,
                        tick.second);
            if (this->_plotGenerator && bar.time % 60 == 0)
                this->_AddPlotData(bar.time, tick);
        }
        if (this->_plotGenerator)
            this->_plotGenerator->WriteToDisk();
    }

    void Task::_AddPlotData(time_t time, std::pair<float, float> const& tick)
    {
        float equity;
        equity = this->_state.balance;
        if (this->_state.status == Core::Controller::StatusBuy)
            equity += this->_strategyInstantiator->GetStrategy()->OffsetToPips(tick.second - this->_state.open) * 10 * this->_state.lots;
        else if (this->_state.status == Core::Controller::StatusSell)
            equity += this->_strategyInstantiator->GetStrategy()->OffsetToPips(this->_state.open - tick.first) * 10 * this->_state.lots;
        this->_plotGenerator->AddData(time, this->_state.balance, equity);
    }

    void Task::_ClosePosition(Core::Bar const& bar, float price, std::string const& reason)
    {
        Report::Trade t;
        t.type = this->_state.status;
        t.open = this->_state.open;
        t.lots = this->_state.lots;
        t.close = price;
        if (this->_state.status == Core::Controller::StatusBuy)
            t.pips = this->_strategyInstantiator->GetStrategy()->OffsetToPips(t.close - t.open);
        else
            t.pips = this->_strategyInstantiator->GetStrategy()->OffsetToPips(t.open - t.close);
        t.counterCurrencyProfit = t.pips * 10 * t.lots;
        t.baseCurrencyProfit = t.counterCurrencyProfit * (1 / t.close);
        t.sl = this->_state.sl;
        t.tp = this->_state.tp;
        this->_report.AddTrade(t);
        if (this->_conf.showTradeActions)
            this->_logger.Log(CLASS "Close " +
                    std::string(this->_state.status == Core::Controller::StatusBuy ? "buy" : "sell") + " at " +
                    bar.TimeToString() + " " +
                    this->_PriceString(price) + ": " +
                    reason + " (" + (t.counterCurrencyProfit > 0 ? "profit" : t.counterCurrencyProfit == 0 ? "even" : "loss") + ").");
        this->_state.balance += t.counterCurrencyProfit;
        this->_ResetState();
    }

    void Task::_ClosePosition(Core::Bar const& bar, std::pair<float, float> const& tick, std::string const& reason)
    {
        if (this->_state.status == Core::Controller::StatusBuy)
            this->_ClosePosition(bar, tick.second, reason);
        else
            this->_ClosePosition(bar, tick.first, reason);
    }

    void Task::_Interrupt(Core::Bar const& bar, std::pair<float, float> const& tick)
    {
        if (this->_state.status != Core::Controller::StatusNothing)
            this->_ClosePosition(bar, tick, "interrupt/gap");
    }

    void Task::_PreTick(Core::Bar const& bar, std::pair<float, float> const& tick)
    {
        if (this->_state.status == Core::Controller::StatusBuy)
        {
            if (tick.second >= this->_state.tp)
                this->_ClosePosition(bar, this->_state.tp, "top TP hit");
            else if (tick.second <= this->_state.sl)
                this->_ClosePosition(bar, this->_state.sl, "bottom SL hit");
        }
        else if (this->_state.status == Core::Controller::StatusSell)
        {
            if (tick.first <= this->_state.tp)
                this->_ClosePosition(bar, this->_state.tp, "bottom TP hit");
            else if (tick.first >= this->_state.sl)
                this->_ClosePosition(bar, this->_state.sl, "top SL hit");
        }
    }

    bool Task::_PostTick(Core::Bar const& bar, std::pair<float, float> const& tick, Core::Controller::Output const& o)
    {
        if (o.order == Core::Controller::OrderNothing)
            return false;
        Core::Strategy::Strategy& s = *this->_strategyInstantiator->GetStrategy();
        float sl = s.RoundPrice(o.sl);
        float tp = s.RoundPrice(o.tp);
        if (this->_state.status == Core::Controller::StatusNothing) // backtester not trading
        {
            // invalid order check
            if (o.order != Core::Controller::OrderBuy && o.order != Core::Controller::OrderSell)
            {
                this->_logger.Log(CLASS "Invalid order " + Tools::ToString(o.order) + " \"" + Core::Controller::ToString(o.order) + "\" while not trading.", ::Logger::Warning);
                return false;
            }
            // invalid lots check
            if (o.lots <= 0)
            {
                this->_logger.Log(CLASS "Invalid lots value: " + Tools::ToString(o.lots) + ".", ::Logger::Warning);
                return false;
            }
            if (o.order == Core::Controller::OrderBuy) // buy
            {
                // invalid sl/tp check
                if (this->_CheckPriceRange(tick, sl) || this->_CheckPriceRange(tick, tp) || tp <= tick.first || sl >= tick.second)
                {
                    this->_logger.Log(CLASS "Invalid SL/TP for buying at " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".", ::Logger::Warning);
                    return false;
                }
                // ok buy
                this->_state.status = Core::Controller::StatusBuy;
                this->_state.open = tick.first; // ask
                if (this->_conf.showTradeActions)
                    this->_logger.Log(CLASS "Buy at " + bar.TimeToString() + " " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".");
            }
            else // sell
            {
                // invalid sl/tp check
                if (this->_CheckPriceRange(tick, sl) || this->_CheckPriceRange(tick, tp) || tp >= tick.second || sl <= tick.first)
                {
                    this->_logger.Log(CLASS "Invalid SL/TP for selling at " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".", ::Logger::Warning);
                    return false;
                }
                // ok sell
                this->_state.status = Core::Controller::StatusSell;
                this->_state.open = tick.second; // bid
                if (this->_conf.showTradeActions)
                    this->_logger.Log(CLASS "Sell at " + bar.TimeToString() + " " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".");
            }
            this->_state.lots = o.lots;
            this->_state.sl = sl;
            this->_state.tp = tp;
        }
        else // backtester trading
        {
            // invalid order check
            if (o.order != Core::Controller::OrderClose && o.order != Core::Controller::OrderAdjust)
            {
                this->_logger.Log(CLASS "Invalid order " + Tools::ToString(o.order) + " \"" + Core::Controller::ToString(o.order) + "\" while trading.", ::Logger::Warning);
                return false;
            }
            if (o.order == Core::Controller::OrderClose) // close
                this->_ClosePosition(bar, tick, "actor order");
            else // adjust
            {
                if (o.order == Core::Controller::OrderBuy) // open position buy
                {
                    // invalid sl/tp check
                    if (this->_CheckPriceRange(tick, sl) || this->_CheckPriceRange(tick, tp) || tp <= tick.first || sl >= tick.second)
                    {
                        this->_logger.Log(CLASS "Invalid SL/TP for adjusting buy at " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".", ::Logger::Warning);
                        return false;
                    }
                }
                else // open position sell
                {
                    // invalid sl/tp check
                    if (this->_CheckPriceRange(tick, sl) || this->_CheckPriceRange(tick, tp) || tp >= tick.second || sl <= tick.first)
                    {
                        this->_logger.Log(CLASS "Invalid SL/TP for adjusting sell at " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".", ::Logger::Warning);
                        return false;
                    }
                }
                // ok adjust
                this->_state.sl = sl;
                this->_state.tp = tp;
                if (this->_conf.showTradeActions)
                    this->_logger.Log(CLASS "Adjusting at " + bar.TimeToString() + " " + this->_PriceString(tick) + " SL " + this->_PriceString(sl) + " TP " + this->_PriceString(tp) + ".");
            }
        }
        return true;
    }

    // returns true if there is a problem
    bool Task::_CheckPriceRange(std::pair<float, float> const& tick, float price) const
    {
        float minPriceOffset = this->_conf.minPriceOffset * this->_strategyInstantiator->GetStrategy()->GetPipPrice();
        float rangeHigher = tick.first + minPriceOffset; // ask + X
        float rangeLower = tick.second - minPriceOffset; // bid - X
        return price <= rangeHigher && price >= rangeLower;
    }

    std::string Task::_PriceString(std::pair<float, float> const& tick) const
    {
        return "[ask " + Tools::ToString(tick.first, this->_strategyInstantiator->GetStrategy()->GetDigits()) +
            ", bid " + Tools::ToString(tick.second, this->_strategyInstantiator->GetStrategy()->GetDigits()) + "]";
    }

    std::string Task::_PriceString(float price) const
    {
        return Tools::ToString(price, this->_strategyInstantiator->GetStrategy()->GetDigits());
    }
}
