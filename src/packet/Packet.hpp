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

#ifndef __PACKET_PACKET__
#define __PACKET_PACKET__

#include <string>
#include <boost/noncopyable.hpp>
#include <cstdint>

namespace Packet
{
    enum PacketType
    {
        Hello = 1,
        Tick = 2,
        Order = 3,
        Bar = 4,
        Welcome = 5,
        Trade = 6,
        Continue = 7,
    };

    class Packet :
        private boost::noncopyable
    {
        public:
            explicit Packet();
            ~Packet();
            size_t BytesLeft() const;
            void Clear();
            void WriteData(char* data, size_t size);
            void WriteByte(uint8_t val);
            void WriteShort(uint16_t val);
            void WriteInt(uint32_t val);
            void WriteFloat(float val);
            void WriteDouble(double val);
            void WriteString(std::string const& val);
            uint8_t ReadByte();
            uint16_t ReadShort();
            uint32_t ReadInt();
            float ReadFloat();
            double ReadDouble();
            std::string ReadString();
            size_t GetSize() const;
            size_t GetCompleteSize() const;
            char const* GetData() const;
            char const* GetCompleteData() const;
        private:
            enum
            {
                InitialSize = 1024,
                SizeStep = 512,
            };
            void _WriteSize();
            void _WriteByte(char byte);
            void _Resize(size_t target);
            char* _data;
            size_t _size;
            size_t _allocSize;
            size_t _offset;
    };
}

#endif
