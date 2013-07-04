// SEDNL - Copyright (c) 2013 Jeremy S. Cochoy
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//        claim that you wrote the original software. If you use this software
//        in a product, an acknowledgment in the product documentation would
//        be appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//        be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//        distribution.

#include "SEDNL/Packet.hpp"
#include "SEDNL/SocketHelp.hpp"

namespace SedNL
{

Packet::Packet()
{}

template<>
Packet& Packet::operator<< <Int8> (Int8 dt)
{
    data.push_back(static_cast<Byte>(Type::Int8));
    data.push_back(dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt8> (UInt8 dt)
{
    data.push_back(static_cast<Byte>(Type::UInt8));
    data.push_back(dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int16>(Int16 dt)
{
    data.push_back(static_cast<Byte>(Type::Int16));
    __push_16(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt16>(UInt16 dt)
{
    data.push_back(static_cast<Byte>(Type::UInt16));
    __push_16(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int32>(Int32 dt)
{
    data.push_back(static_cast<Byte>(Type::Int32));
    __push_32(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt32>(UInt32 dt)
{
    data.push_back(static_cast<Byte>(Type::UInt32));
    __push_32(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int64>(Int64 dt)
{
    data.push_back(static_cast<Byte>(Type::Int64));
    __push_64(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt64>(UInt64 dt)
{
    data.push_back(static_cast<Byte>(Type::UInt64));
    __push_64(data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <float>(float dt)
{
    data.push_back(static_cast<Byte>(Type::Float));
    __push_32(data, static_cast<UInt32>(dt));
    return *this;
}

template<>
Packet& Packet::operator<< <double>(double dt)
{
    data.push_back(static_cast<Byte>(Type::Double));
    __push_64(data, static_cast<UInt64>(dt));
    return *this;
}

template<>
Packet& Packet::operator<< <const char*>(const char* dt)
{
    data.push_back(static_cast<Byte>(Type::String));
    while (*dt != 0)
        data.push_back(*dt++);
    data.push_back('\0');
    return *this;
}

template<>
Packet& Packet::operator<< <std::string>(std::string dt)
{
    return (*this) << dt.c_str();
}

const ByteArray& Packet::get_data() const
{
    return data;
}

} // namespace SedNL
