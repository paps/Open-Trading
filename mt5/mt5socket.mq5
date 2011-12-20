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

#property copyright   "Martin Tapia"
#property link        "http://open-trading.org"
#property version     "1.2"
#property description "mt5socket"

#include <Trade\Trade.mqh>

input int gMagicNumber = 0;
input string gHost = "192.168.1.103";
input string gPort = "40100";
input string gPassword = "test";

struct DllInput
{
    int error;
    float open;
    float high;
    float low;
    float close;
    float ask;
    float bid;
    int status;
    int newBar;
};

struct DllServer
{
    uint neededBars;
};

struct DllOutput
{
    int order;
    float lots;
    float sl;
    float tp;
};

struct DllTrade
{
    int status;
    float open;
    float lots;
    float sl;
    float tp;
    float askRequote;
    float bidRequote;
};

#import "mt5socket.dll"
int DllStart(string host, string port, string pair, int periodSeconds, int digits, string password, DllServer& server);
int DllAddBar(float& open, float& high, float& low, float& close);
int DllStop();
int DllTick(DllInput& in, DllOutput& out);
int DllTradeInfo(DllTrade& trade);
#import

class DllWrapper
{
    public:
        DllWrapper();
        ~DllWrapper();
        bool Start(ulong magic, ENUM_TIMEFRAMES timeframe, string symbol = "");
        void Stop();
        void Tick();
    private:
        void _GatherInput();
        bool _SendPreviousBars();
        int _NewBar();
        int _GetPositionStatus();
        void _SendTradeInfo(int status, double open, double lots, double sl, double tp, double askRequote, double bidRequote);
        bool _ClosePosition();
        bool _OpenPosition(bool buy, double lots, double sl, double tp);
        bool _ModifyPosition(double sl, double tp);
        DllInput _input;
        DllServer _server;
        DllOutput _output;
        CTrade _trade;
        bool _buyPosition;
        bool _sellPosition;
        CSymbolInfo _sym;
        ulong _magic;
        ENUM_TIMEFRAMES _timeframe;
        datetime _prevBarTime;
};

DllWrapper gExpert;

void Err(string error)
{
    Print(error, " [", GetLastError(), "]");
}

int DllWrapper::_NewBar()
{
    datetime time;
    if (SeriesInfoInteger(_sym.Name(), _timeframe, SERIES_LASTBAR_DATE, time))
    {
        if (_prevBarTime == 0)
        {
            _prevBarTime = time;
            return 0;
        }
        if (_prevBarTime < time)
        {
            _prevBarTime = time;
            return 1;
        }
        return 0;
    }
    return -1;
}

int DllWrapper::_GetPositionStatus()
{
    if (PositionSelect(_sym.Name()))
    {
        if (_buyPosition)
            return 1; // buy
        else if (_sellPosition)
            return 2; // sell
        else
            return 3; // unknown
    }
    else
        return 0; // no open position
}

void DllWrapper::_GatherInput()
{
    _input.error = 0;

    // tick
    MqlTick tick;
    if (SymbolInfoTick(_sym.Name(), tick))
    {
        _input.ask = (float)tick.ask;
        _input.bid = (float)tick.bid;
    }
    else
    {
        Err("Failed to get tick information.");
        _input.error = 1;
    }

    // rates
    MqlRates rates[1];
    if (CopyRates(_sym.Name(), _timeframe, 0, 1, rates) != 1)
    {
        Err("Failed to get the two last rates.");
        _input.error = 1;
    }
    else
    {
        _input.newBar = _NewBar();
        if (_input.newBar < 0)
        {
            Print("Failed to detect new bar.");
            _input.newBar = 0;
            _input.error = 1;
        }
        _input.open = (float)rates[0].open;
        _input.high = (float)rates[0].high;
        _input.low = (float)rates[0].low;
        _input.close = (float)rates[0].close;
    }

    // status
    _input.status = _GetPositionStatus();
}

void DllWrapper::Tick()
{
    _sym.RefreshRates();
    _sym.Refresh();
    _GatherInput();
    int tickRet = DllTick(_input, _output);
    if (tickRet < 0) // error
    {
        Alert("Failed to send Tick packet / receive Order packet.");
        ExpertRemove();
        return;
    }
    else if (tickRet == 1) // Continue packet
    {
        Print("Server: continue (unpaused).");
        return;
    }
    // Order packet
    Print("Server: order ", _output.order, ", lots ", _output.lots, ", sl ", _output.sl, ", tp ", _output.tp);
    // 0 - nothing
    // 1 - buy
    // 2 - sell
    // 3 - close
    // 4 - adjust sl/tp
    if (_output.order == 0) // nothing
        Print("Nothing to do.");
    else if (_output.order == 1 || _output.order == 2) // buy/sell
    {
        if (PositionSelect(_sym.Name()))
        {
            Print("New trade order with an open position... closing.");
            _ClosePosition();
        }
        if (_output.order == 1) // buy
        {
            Print("Opening buy position.");
            _OpenPosition(true, _output.lots, _output.sl, _output.tp);
        }
        else // sell
        {
            Print("Opening sell position.");
            _OpenPosition(false, _output.lots, _output.sl, _output.tp);
        }
    }
    else if (_output.order == 3) // close
    {
        if (PositionSelect(_sym.Name()))
        {
            Print("Closing position.");
            _ClosePosition();
        }
        else
            Print("Close order but no open position.");
    }
    else if (_output.order == 4) // adjust
    {
        if (PositionSelect(_sym.Name()))
        {
            Print("Modifying position, SL=", _output.sl, ", TP=", _output.tp, ".");
            _ModifyPosition(_output.sl, _output.tp);
        }
        else
            Print("Modify order but no open position.");
    }
    else
        Print("Unknown order.");
}

void DllWrapper::_SendTradeInfo(int status, double open, double lots, double sl, double tp, double askRequote, double bidRequote)
{
    DllTrade trade;
    trade.status = status;
    trade.open = (float)open;
    trade.lots = (float)lots;
    trade.sl = (float)sl;
    trade.tp = (float)tp;
    if (DllTradeInfo(trade) < 0)
    {
        Alert("Failed to send Trade packet.");
        ExpertRemove();
        return;
    }
}

bool DllWrapper::_ModifyPosition(double sl,double tp)
{
    sl = _sym.NormalizePrice(sl);
    tp = _sym.NormalizePrice(tp);
    if (_trade.PositionModify(_sym.Name(), sl, tp))
    {
        if (_trade.ResultRetcode() != TRADE_RETCODE_PLACED && _trade.ResultRetcode() != TRADE_RETCODE_DONE)
        {
            Print("Modifiying position failed with return code ", _trade.ResultRetcode(), ".");
            return false;
        }
        Print("Position modified.");
        _SendTradeInfo(_GetPositionStatus(), -1.0, -1.0, sl, tp, -1.0, -1.0);
        return true;
    }
    else
        Err("Failed to modify position.");
    return false;
}

bool DllWrapper::_OpenPosition(bool buy, double lots, double sl, double tp)
{
    _sym.RefreshRates();
    _sym.Refresh();
    ENUM_ORDER_TYPE type;
    sl = _sym.NormalizePrice(sl);
    tp = _sym.NormalizePrice(tp);
    double price;
    if (buy)
    {
        type = ORDER_TYPE_BUY;
        price = _sym.Ask();
    }
    else
    {
        type = ORDER_TYPE_SELL;
        price = _sym.Bid();
    }
    if (_trade.PositionOpen(_sym.Name(), type, lots, price, sl, tp))
    {
        if (_trade.ResultRetcode() != TRADE_RETCODE_PLACED && _trade.ResultRetcode() != TRADE_RETCODE_DONE)
        {
            Print("Opening position failed with return code ", _trade.ResultRetcode(), ".");
            return false;
        }
        Print("Position opened.");
        if (buy)
        {
            _buyPosition = true;
            _sellPosition = false;
            _SendTradeInfo(1, _trade.ResultPrice(), _trade.ResultVolume(), sl, tp, _trade.ResultAsk(), _trade.ResultBid()); // 1 = buy
        }
        else
        {
            _buyPosition = false;
            _sellPosition = true;
            _SendTradeInfo(2, _trade.ResultPrice(), _trade.ResultVolume(), sl, tp, _trade.ResultAsk(), _trade.ResultBid()); // 2 = sell
        }
        return true;
    }
    else
        Err("Failed to open position.");
    return false;
}

bool DllWrapper::_ClosePosition()
{
    if (_trade.PositionClose(_sym.Name()))
    {
        if (_trade.ResultRetcode() != TRADE_RETCODE_PLACED && _trade.ResultRetcode() != TRADE_RETCODE_DONE)
        {
            Print("Failed to close position with return code ", _trade.ResultRetcode(), ".");
            return false;
        }
        Print("Position closed.");
        _sellPosition = false;
        _buyPosition = false;
        _SendTradeInfo(0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0); // 0 = nothing
        return true;
    }
    Err("Failed to close position.");
    return false;
}

DllWrapper::DllWrapper()
{
    Print("-> DllWrapper::DllWrapper()");
    _prevBarTime = 0;
    _buyPosition = false;
    _sellPosition = false;
}

DllWrapper::~DllWrapper()
{
    Print("-> DllWrapper::~DllWrapper()");
}

bool DllWrapper::_SendPreviousBars()
{
    if (MQL5InfoInteger(MQL5_TESTING))
    {
        Print("Testing mode: previous bars not sent.");
        return true;
    }
    MqlRates rates[250];
    if (CopyRates(_sym.Name(), _timeframe, 1, 250, rates) != 250)
    {
        Err("Failed to get the last 250 rates.");
        return false;
    }
    if (_server.neededBars > 250)
    {
        Print("Server asked for more than 250 previous bars.");
        return false;
    }
    Print("Sending ", _server.neededBars, " previous bars...");
    uint startIndex = 250 - _server.neededBars;
    for (uint i = startIndex; i < 250; ++i)
    {
        float open = (float)rates[i].open;
        float high = (float)rates[i].high;
        float low = (float)rates[i].low;
        float close = (float)rates[i].close;
        if (DllAddBar(open, high, low, close) < 0)
        {
            Alert("Could not send all the previous bars.");
            return false;
        }
    }
    return true;
}

bool DllWrapper::Start(ulong magic, ENUM_TIMEFRAMES timeframe, string symbol /* = "" */)
{
    Print("-> DllWrapper::Start()");
    _magic = magic;
    _trade.SetExpertMagicNumber(_magic);
    _trade.SetDeviationInPoints(100); // ... :(
    if (symbol == "")
        _sym.Name(Symbol());
    else
        _sym.Name(symbol);
    string symbolPath;
    if (SymbolInfoString(_sym.Name(), SYMBOL_PATH, symbolPath))
        Print("Sucessfully selected symbol \"", _sym.Name(), "\" (", symbolPath, ").");
    else
    {
        Print("Failed to select symbol \"", symbol, "\".");
        return false;
    }
    _timeframe = timeframe;
    if (DllStart(gHost, gPort, _sym.Name(), PeriodSeconds(_timeframe), _sym.Digits(), gPassword, _server) < 0)
    {
        Alert("Connection failed to " + gHost + ":" + gPort + ".");
        return false;
    }
    return _SendPreviousBars();
}

void DllWrapper::Stop()
{
    Print("-> DllWrapper::Stop()");
    if (DllStop() < 0)
        Alert("Failed to stop.");
}

int OnInit()
{
    if (!gExpert.Start(gMagicNumber, PERIOD_CURRENT))
        return -1;
    return 0;
}

void OnDeinit(const int reason)
{
    gExpert.Stop();
}

void OnTick()
{
    gExpert.Tick();
}
