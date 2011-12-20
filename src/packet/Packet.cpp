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

#include <stdexcept>
#include <cstring>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include "Packet.hpp"

namespace Packet
{
    Packet::Packet() :
        _size(0), _allocSize(InitialSize), _offset(2)
    {
        this->_data = new char[InitialSize];
        this->_WriteSize();
    }

    Packet::~Packet()
    {
        delete [] this->_data;
    }

    size_t Packet::BytesLeft() const
    {
        return this->_size - this->_offset + 2;
    }

    void Packet::WriteString(std::string const& val)
    {
        this->WriteShort(val.size());
        for (size_t i = 0; i != val.size(); ++i)
            this->_WriteByte(val[i]);
    }

    void Packet::WriteByte(uint8_t val)
    {
        this->_WriteByte(static_cast<char>(val));
    }

    void Packet::WriteShort(uint16_t val)
    {
        val = htons(val);
        for (unsigned int i = 0; i < 2; ++i)
            this->_WriteByte(*(reinterpret_cast<char*>(&val) + i));
    }

    void Packet::WriteInt(uint32_t val)
    {
        val = htonl(val);
        for (unsigned int i = 0; i < 4; ++i)
            this->_WriteByte(*(reinterpret_cast<char*>(&val) + i));
    }

    void Packet::WriteFloat(float val)
    {
        for (unsigned int i = 0; i < 4; ++i)
            this->_WriteByte(*(reinterpret_cast<char*>(&val) + i));
    }

    void Packet::WriteDouble(double val)
    {
        for (unsigned int i = 0; i < 8; i++)
            this->_WriteByte(*(reinterpret_cast<char*>(&val) + i));
    }

    uint8_t Packet::ReadByte()
    {
        if (this->_offset < this->_size + 2)
            return this->_data[this->_offset++];
        throw std::runtime_error("packet too small to read");
    }

    uint16_t Packet::ReadShort()
    {
        if (this->_offset + 1 < this->_size + 2)
        {
            uint16_t const* ret = reinterpret_cast<uint16_t const*>(&this->_data[this->_offset]);
            this->_offset += 2;
            return ntohs(*ret);
        }
        throw std::runtime_error("packet too small to read");
    }

    uint32_t Packet::ReadInt()
    {
        if (this->_offset + 3 < this->_size + 2)
        {
            uint32_t const* ret = reinterpret_cast<uint32_t const*>(&this->_data[this->_offset]);
            this->_offset += 4;
            return ntohl(*ret);
        }
        throw std::runtime_error("packet too small to read");
    }

    float Packet::ReadFloat()
    {
        if (this->_offset + 3 < this->_size + 2)
        {
            float const* ret = reinterpret_cast<float const*>(&this->_data[this->_offset]);
            this->_offset += 4;
            return *ret;
        }
        throw std::runtime_error("packet too small to read");
    }

    double Packet::ReadDouble()
    {
        if (this->_offset + 7 < this->_size + 2)
        {
            double const* ret = reinterpret_cast<double const*>(&this->_data[this->_offset]);
            this->_offset += 8;
            return *ret;
        }
        throw std::runtime_error("packet too small to read");
    }

    std::string Packet::ReadString()
    {
        size_t size = this->ReadShort();
        std::string ret;
        for (size_t i = 0; i < size; ++i)
            ret += this->ReadByte();
        return ret;
    }

    void Packet::Clear()
    {
        this->_size = 0;
        this->_offset = 2;
        this->_WriteSize();
    }

    void Packet::WriteData(char* data, size_t size)
    {
        if (this->_allocSize < size + 2)
            this->_Resize(size + 2);
        ::memcpy(this->_data + 2, data, size);
        this->_size = size;
        this->_offset = 2;
        this->_WriteSize();
    }

    void Packet::_Resize(size_t target)
    {
        char* tmp = this->_data;
        this->_data = new char[target];
        ::memcpy(this->_data, tmp, this->_allocSize);
        this->_allocSize = target;
        delete [] tmp;
    }

    void Packet::_WriteByte(char byte)
    {
        if (this->_offset >= _allocSize)
            this->_Resize(_allocSize + SizeStep);
        this->_data[this->_offset] = byte;
        ++this->_offset;
        if (this->_offset - 2 > this->_size)
            this->_size = this->_offset - 2;
        this->_WriteSize();
    }

    void Packet::_WriteSize()
    {
        uint16_t size = htons(static_cast<uint16_t>(this->_size));
        this->_data[0] = *(reinterpret_cast<char*>(&size));
        this->_data[1] = *(reinterpret_cast<char*>(&size) + 1);
    }

    char const* Packet::GetData() const
    {
        return this->_data + 2;
    }

    char const* Packet::GetCompleteData() const
    {
        return this->_data;
    }

    size_t Packet::GetSize() const
    {
        return this->_size;
    }

    size_t Packet::GetCompleteSize() const
    {
        return this->_size + 2;
    }
}
