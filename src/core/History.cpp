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
#include "History.hpp"
#include "logger/Logger.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Core/History] "

namespace Core
{
    History::History(Logger::Logger const& logger, bool showErrors /* = false */) :
        _logger(logger), _maxGapSize(0), _showErrors(showErrors)
    {
    }

    void History::CopyDataFrom(History const& history)
    {
        this->_path = history.GetPath();
        this->_maxGapSize = history.GetMaxGapSize();
        this->_bars = history.GetBars();
    }

    std::string const& History::GetPath() const
    {
        return this->_path;
    }

    std::vector<Bar> const& History::GetBars() const
    {
        return this->_bars;
    }

    unsigned int History::GetMaxGapSize() const
    {
        return this->_maxGapSize;
    }

    History::FetchType History::FetchBar(Bar& bar, unsigned int pos, unsigned int period) const
    {
		if (period == 0 || this->_bars.size() <= pos + period)
        {
            bar.valid = false;
            return FetchError;
        }
        bar.o = this->_bars[pos].o;
        bar.h = -1000000.0;
        bar.l = 1000000.0; // :( ...
        bar.time = this->_bars[pos].time;
        for (unsigned int i = pos; i < pos + period; ++i)
            if (this->_bars[i].valid)
            {
                if (this->_bars[i].h > bar.h)
                    bar.h = this->_bars[i].h;
                if (this->_bars[i].l < bar.l)
                    bar.l = this->_bars[i].l;
            }
            else
            {
                bar.valid = false;
                return FetchGap;
            }
        bar.c = this->_bars[pos + period - 1].c;
        bar.valid = true;
        return FetchOk;
    }

    unsigned int History::GetFirstBarPosOfPeriod(unsigned int period) const
    {
        if (!period)
            return 0;
        unsigned int pos = 0;
        unsigned int secs = period * 60;
        while (pos < this->_bars.size() && this->_bars[pos].time % secs)
            ++pos;
        return pos;
    }

    unsigned int History::GetBarPosFromDate(time_t time, bool& success) const
    {
        unsigned int pos;
        std::vector<Bar>::const_iterator it = this->_bars.begin();
        std::vector<Bar>::const_iterator itEnd = this->_bars.end();
        for (; it != itEnd; ++it)
            if (it->time == time)
            {
                success = true;
                return pos;
            }
            else
                ++pos;
        success = false;
        return 0;
    }

    unsigned int History::Load(std::string const& path, unsigned int maxGapSize /* = 60 */)
    {
        this->_bars.clear();
        this->_path = path;
        this->_maxGapSize = maxGapSize;
        Bar previousBar;
        char buf[512];
        unsigned int bars = 0;
        unsigned int fail = 0;
        unsigned int gaps = 0;
        unsigned int generatedBars = 0;
        unsigned int generatedGaps = 0;
        unsigned int invalidBars = 0;
        unsigned int lineNumber = 0;
        this->_logger.Log(CLASS "Loading \"" + this->_path + "\" (maximum gap size of " + Tools::ToString(this->_maxGapSize) + " bars)...");
        std::ifstream file(this->_path.c_str());
        if (!file.good())
            this->_logger.Log(CLASS "Failed to open history file \"" + this->_path + "\".", Logger::Error);
        while (file.good())
        {
            ++lineNumber;
            file.getline(buf, 512);
            std::string line(buf);
            if (line.size() > 17) // 2010.09.02,01:24, ... = 17 chars
            {
                Bar bar;
                this->_FetchTimeFromCsv(line, bar);
                line += '\n'; // shitty but works
                this->_FetchValuesFromCsv(line, bar);
                if (bar.valid)
                {
                    if (previousBar.valid) // this is not the first bar to be read
                    {
                        unsigned int offset = (bar.time - previousBar.time) / 60;
                        if (offset > 3500) // gap > week end
                        {
                            this->_logger.Log(CLASS "Gap of " + Tools::ToString(offset) + " minutes in history \"" + path + "\". Loading aborted.", Logger::Error);
                            this->_bars.clear();
                            bars = 0;
                            break;
                        }
                        else if (offset == 1) // normal space between bars
                            this->_bars.push_back(bar);
                        else if (offset == 0) // weird
                            this->_logger.Log(CLASS "Zero minute gap at " + bar.TimeToString() + ", bar ignored.");
                        else if (offset <= this->_maxGapSize) // no gap but generated bars
                        {
                            generatedBars += offset - 1;
                            ++generatedGaps;
                            Bar genBar;
                            genBar.o = previousBar.c;
                            genBar.h = previousBar.c;
                            genBar.l = previousBar.c;
                            genBar.c = previousBar.c;
                            genBar.valid = true;
                            genBar.time = previousBar.time;
                            for (unsigned int i = 0; i < offset - 1; ++i)
                            {
                                genBar.time += 60;
                                this->_bars.push_back(genBar);
                            }
                            this->_bars.push_back(bar);
                        }
                        else // gap filled with invalid bars
                        {
                            invalidBars += offset - 1;
                            ++gaps;
                            time_t t = previousBar.time;
                            for (unsigned int i = 0; i < offset - 1; ++i)
                            {
                                t += 60;
                                Bar gap; // is invalid by default
                                gap.time = t;
                                this->_bars.push_back(gap);
                            }
                            this->_bars.push_back(bar);
                        }
                    }
                    else // first bar to be read
                        this->_bars.push_back(bar);
                    previousBar = bar;
                    ++bars;
                }
                else
                {
                    if (this->_showErrors)
                        this->_logger.Log(CLASS "Line " + Tools::ToString(lineNumber) + ": could not parse OHLC values.", Logger::Warning);
                    ++fail;
                }
            }
            else
            {
                if (this->_showErrors)
                    this->_logger.Log(CLASS "Line " + Tools::ToString(lineNumber) + ": could not parse date.", Logger::Warning);
                ++fail;
            }
        }
        if (bars == 0)
            this->_logger.Log(CLASS "Loading of history \"" + this->_path + "\" failed.", Logger::Error);
        else
        {
            this->_logger.Log(CLASS "\"" + this->_path + "\" loaded: "
                    + Tools::ToString(bars) + " bars, "
                    + Tools::ToString(fail) + " invalid lines, "
                    + Tools::ToString(gaps) + " real gaps (" + Tools::ToString(invalidBars) + " invalid bars), "
                    + Tools::ToString(generatedGaps) + " fixed gaps (" + Tools::ToString(generatedBars) + " generated bars).");
            this->_ShowTransitionQuality();
        }
        return this->_VerifyHistory();
    }

    unsigned int History::_VerifyHistory()
    {
        if (this->_bars.size() == 0)
            return 0;
        unsigned int bars = 0;
        unsigned int validBars = 0;
        unsigned int invalidBars = 0;
        time_t previousTime = 0;
        std::vector<Bar>::iterator it = this->_bars.begin();
        std::vector<Bar>::iterator itEnd = this->_bars.end();
        for (; it != itEnd; ++it)
        {
            ++bars;
            if (previousTime != 0)
            {
                if (it->time - previousTime != 60)
                    ++invalidBars;
                else
                    ++validBars;
            }
            else
                ++validBars;
            previousTime = it->time;
        }
        this->_logger.Log(CLASS "\"" + this->_path + "\" integrity: "
                + Tools::ToString(bars) + " bars, "
                + Tools::ToString(validBars) + " continuous bars, "
                + Tools::ToString(invalidBars) + " non-continuous bars.");
        if (invalidBars > 0)
        {
            this->_logger.Log(CLASS "Integry check for \"" + this->_path + "\" failed.", Logger::Error);
            this->_bars.clear();
            return 0;
        }
        return this->_bars.size();
    }

    void History::_ShowTransitionQuality() const
    {
        bool first = true;
        Bar prev;
        unsigned int valid = 0;
        unsigned int total = 0;
        std::vector<Bar>::const_iterator it = this->_bars.begin();
        std::vector<Bar>::const_iterator itEnd = this->_bars.end();
        for (; it != itEnd; ++it)
        {
            if (first)
                first = false;
            else
            {
                if (prev.c == it->o)
                    ++valid;
                ++total;
            }
            prev = *it;
        }
        if (total > 0)
            this->_logger.Log(CLASS "\"" + this->_path + "\" transitions: "
                    + Tools::ToString(total) + " total, "
                    + Tools::ToString(valid) + " continuous ("
                    + Tools::ToString((static_cast<float>(valid) / static_cast<float>(total)) * 100.0, 1) + "%).");
    }

    void History::_FetchValuesFromCsv(std::string const& line, Bar& bar) const
    {
        float val[4];
        int i = 0;
        std::string buf;
        std::string::const_iterator it = line.begin() + 17;
        for (; it != line.end(); ++it)
            if (*it == ',' || *it == '\n' || *it == '\r')
            {
                val[i++] = atof(buf.c_str());
                if (i >= 4)
                {
                    if (val[0] > 0 && val[1] > 0 && val[2] > 0 && val[3] > 0)
                    {
                        bar.o = val[0];
                        bar.h = val[1];
                        bar.l = val[2];
                        bar.c = val[3];
                        if (bar.h >= bar.l && bar.o <= bar.h && bar.o >= bar.l && bar.c <= bar.h && bar.c >= bar.l)
                            bar.valid = true;
                    }
                    return;
                }
                buf.clear();
            }
            else
                buf.push_back(*it);
    }

    void History::_FetchTimeFromCsv(std::string const& line, Bar& bar) const
    {
        struct tm timeinfo;
        timeinfo.tm_isdst = -1;
        timeinfo.tm_sec = 0;
        std::string year;
        year.push_back(line[0]);
        year.push_back(line[1]);
        year.push_back(line[2]);
        year.push_back(line[3]);
        timeinfo.tm_year = atoi(year.c_str()) - 1900;
        std::string month;
        month.push_back(line[5]);
        month.push_back(line[6]);
        timeinfo.tm_mon = atoi(month.c_str()) - 1;
        std::string day;
		day.push_back(line[8]);
		day.push_back(line[9]);
		timeinfo.tm_mday = atoi(day.c_str());
		std::string hour;
		hour.push_back(line[11]);
		hour.push_back(line[12]);
		timeinfo.tm_hour = atoi(hour.c_str());
		std::string min;
		min.push_back(line[14]);
		min.push_back(line[15]);
		timeinfo.tm_min = atoi(min.c_str());
		bar.time = mktime(&timeinfo);
	}
}
