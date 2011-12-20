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
#property version     "1.0"
#property description "mt5history"

#include <Trade\Trade.mqh>

input string gPath = "capture.csv";

class HistoryExtractor
{
    public:
        HistoryExtractor();
        ~HistoryExtractor();
        bool Start(ENUM_TIMEFRAMES timeframe, string symbol = "");
        void Stop();
        void Tick();
    private:
        int _NewBar();
        CSymbolInfo _sym;
        ENUM_TIMEFRAMES _timeframe;
        datetime _prevBarTime;
        int _file;
};

HistoryExtractor gExpert;

void Die(string error)
{
    Alert(error);
    ExpertRemove();
}

int HistoryExtractor::_NewBar()
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

void HistoryExtractor::Tick()
{
    _sym.RefreshRates();
    _sym.Refresh();
    int newBar = _NewBar();
    if (newBar > 0)
    {
        MqlRates rate[1];
        if (CopyRates(_sym.Name(), _timeframe, 1, 1, rate) != 1)
            Die("CopyRates() fail");
        MqlDateTime date;
        TimeToStruct(rate[0].time, date);
        FileWrite(_file, StringFormat("%d.%02d.%02d", date.year, date.mon, date.day),
                StringFormat("%02d:%02d", date.hour, date.min),
                rate[0].open, rate[0].high, rate[0].low, rate[0].close);
    }
    //else if (newBar < 0)
    //    Die("_NewBar() fail");
}

HistoryExtractor::HistoryExtractor()
{
    Print("-> HistoryExtractor()");
    _prevBarTime = 0;
}

HistoryExtractor::~HistoryExtractor()
{
    Print("-> ~HistoryExtractor()");
}

bool HistoryExtractor::Start(ENUM_TIMEFRAMES timeframe, string symbol /* = "" */)
{
    Print("-> Start()");
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
    //if (_timeframe != PERIOD_M1)
    //{
    //    Print("Invalid timeframe (expected M1).");
    //    return false;
    //}
    _file = FileOpen(gPath, FILE_WRITE | FILE_CSV | FILE_ANSI, ',');
    if (_file == INVALID_HANDLE)
    {
        Print("Failed to open file \"", gPath, "\" for writing.");
        return false;
    }
    return true;
}

void HistoryExtractor::Stop()
{
    Print("-> Stop()");
    FileClose(_file);
}

int OnInit()
{
    if (!gExpert.Start(PERIOD_CURRENT))
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
