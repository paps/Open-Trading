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

#include "Bar.hpp"
#include "logger/Logger.hpp"
#include "tools/ToString.hpp"
#include "tools/TimeToString.hpp"

namespace Core
{
    Bar::Bar() :
        valid(false)
    {
    }

    Bar::Bar(float o, float h, float l, float c, time_t time /* = 0 */, bool valid /* = false */) :
        o(o), h(h), l(l), c(c), time(time), valid(valid)
    {
    }

    std::string Bar::TimeToString() const
    {
        return Tools::TimeToString(this->time);
    }

    std::string Bar::ValuesToString(unsigned int digits /* = 5 */) const
    {
        return "O " + Tools::ToString(this->o, digits)
            + " H " + Tools::ToString(this->h, digits)
            + " L " + Tools::ToString(this->l, digits)
            + " C " + Tools::ToString(this->c, digits);
    }
}
