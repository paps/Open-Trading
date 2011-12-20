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

#ifndef __CORE_HISTORY__
#define __CORE_HISTORY__

#include <boost/noncopyable.hpp>
#include <string>
#include <vector>
#include "Bar.hpp"

namespace Logger
{
    class Logger;
}

namespace Core
{
    class History :
        private boost::noncopyable
    {
        public:
            explicit History(Logger::Logger const& logger, bool showErrors = false);

            /*
               Erases the current history and loads another file.
               Returns the number of bars loaded (0 -> failure).
               If a gap exceeds maxGapSize 1 minute bars, it is filled with invalid bars.
             */
            unsigned int Load(std::string const& path, unsigned int maxGapSize = 60);

            /*
               Returns the name of the last loaded file.
             */
            std::string const& GetPath() const;

            /*
               Returns the data.
             */
            std::vector<Bar> const& GetBars() const;

            /*
               Gets a bar of a certain period from the data. pos is in 1 minute bars.
               If period is 0 or if pos is out of bounds, returns FetchError with an invalid bar (no OHLC set).
               If there is a gap, returns FetchGap with an invalid bar (no OHLC set).
               Otherwise returns FetchOk with a valid bar.
             */
            enum FetchType
            {
                FetchOk = 0,
                FetchError,
                FetchGap,
            };
			FetchType FetchBar(Bar& bar, unsigned int pos, unsigned int period) const;

            /*
               Finds a position in 1 minute bars from a date.
               If success is false, the date was not found in the history and the return value should be ignored.
             */
            unsigned int GetBarPosFromDate(time_t time, bool& success) const;

            /*
               Returns a position in 1 minute bars corresponding to the beginning of the first bar of period.
               It may return an invalid value if the first bar of period is too close to the end.
             */
            unsigned int GetFirstBarPosOfPeriod(unsigned int period) const;

            /*
               Returns the maximum gap size.
            */
            unsigned int GetMaxGapSize() const;

            /*
               Copies all the data from another history object.
            */
            void CopyDataFrom(History const& history);

        private:
            unsigned int _VerifyHistory();
            void _ShowTransitionQuality() const;
            void _FetchValuesFromCsv(std::string const& line, Bar& bar) const;
            void _FetchTimeFromCsv(std::string const& line, Bar& bar) const;
            Logger::Logger const& _logger;
            std::vector<Bar> _bars;
            std::string _path;
            unsigned int _maxGapSize;
            bool _showErrors;
    };
}

#endif
