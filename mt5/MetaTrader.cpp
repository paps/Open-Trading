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

#include <string>
#include <locale>
#include <vector>
#include "MetaTrader.hpp"
#include "Client.hpp"

static std::string narrow(const std::wstring& ws)
{
    std::vector<char> buffer(ws.size());
    std::locale loc("english");
    std::use_facet< std::ctype<wchar_t> >(loc).narrow(ws.data(), ws.data() + ws.size(), '?', &buffer[0]);
    return std::string(&buffer[0], buffer.size());
}

Client* gClient;

EXPORT int __stdcall DllStart(wchar_t* host, wchar_t* port, wchar_t* historyFile, int periodSeconds, int digits, wchar_t* password, MetaTrader::Server& server)
{
	try
	{
		gClient = new Client(narrow(host), narrow(port), narrow(historyFile), periodSeconds, digits, narrow(password), server);
	}
	catch (std::exception&)
	{
		return -1;
	}
	return 0;
}

EXPORT int __stdcall DllAddBar(float& open, float& high, float& low, float& close)
{
	try
	{
		gClient->AddBar(open, high, low, close);
	}
	catch (std::exception&)
	{
		return -1;
	}
	return 0;
}

EXPORT int __stdcall DllStop()
{
	try
	{
		delete gClient;
	}
	catch (std::exception&)
	{
		return -1;
	}
	return 0;
}

EXPORT int __stdcall DllTradeInfo(MetaTrader::Trade& trade)
{
	try
	{
		gClient->TradeInfo(trade);
	}
	catch (std::exception&)
	{
		return -1;
	}
	return 0;
}

// returns -1 on error
// 1 on Continue packet
// 0 on Order packet
EXPORT int __stdcall DllTick(MetaTrader::Input& in, MetaTrader::Output& out)
{
	try
	{
		if (gClient->Tick(in, out))
			return 1;
	}
	catch (std::exception&)
	{
		return -1;
	}
	return 0;
}
