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

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <arpa/inet.h>
#include <netinet/in.h>

typedef uint64_t n_64;
typedef uint32_t n_32;
typedef uint16_t n_16;

#endif /* SEDNL_WINDOWS */

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

static inline
void __push_16(ByteArray& data, UInt16 dt)
{
    const n_16 ndt = htons(static_cast<n_16>(dt));
    const Byte* const bytes = reinterpret_cast<const Byte*>(&ndt);
    data.push_back(bytes[0]);
    data.push_back(bytes[1]);
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

static inline
void __push_32(ByteArray& data, UInt32 dt)
{
    const n_32 ndt = htonl(static_cast<n_32>(dt));
    const Byte* const bytes = reinterpret_cast<const Byte*>(&ndt);
    data.push_back(bytes[0]);
    data.push_back(bytes[1]);
    data.push_back(bytes[2]);
    data.push_back(bytes[3]);
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

static inline
bool __is_big_endian()
{
    const n_64 v = 1;

    return (*reinterpret_cast<const char*>(&v) == 0);
}

static inline
void __bytes_swap(UInt64&)
{
}

static inline
void __push_64(ByteArray& data, UInt64 dt)
{
    if (!__is_big_endian())
        __bytes_swap(dt);
    __push_32(data, static_cast<UInt32>(dt >> 32));
    __push_32(data, static_cast<UInt32>(dt));
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

UInt16 Packet::get_network_length() const
{
    const n_16 length = htons(static_cast<UInt16>(data.size()));
    return static_cast<UInt16>(length);
}

} // namespace SedNL
