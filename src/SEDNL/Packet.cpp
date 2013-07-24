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
#include <iostream>//DEBUG
namespace SedNL
{

Packet::Packet()
{}

bool Packet::is_valid() noexcept
{
    unsigned int size = m_data.size();

    unsigned int i = 0;
    for (; i < size; i++)
    {
        Type t = static_cast<Type>(m_data[i]);

        switch(t)
        {
        case Type::Int8:
            i += sizeof(Int8);
            break;
        case Type::Int16:
            i += sizeof(Int16);
            break;
        case Type::Int32:
            i += sizeof(Int32);
            break;
        case Type::Int64:
            i += sizeof(Int64);
            break;
        case Type::UInt8:
            i += sizeof(UInt8);
            break;
        case Type::UInt16:
            i += sizeof(UInt16);
            break;
        case Type::UInt32:
            i += sizeof(UInt32);
            break;
        case Type::UInt64:
            i += sizeof(UInt64);
            break;
        case Type::Float:
            i += sizeof(UInt32);
            break;
        case Type::Double:
            i += sizeof(UInt64);
            break;
        case Type::String:
        {
            i++;
            while (m_data[i] != '\0' && i < size)
                i++;
            if (i == size)
                return false;
            break;
        }
        case Type::Object:
            break;
        default:
            return false;
        }
    }

    if (i != size)
        return false;

    return true;
}

///////////
// INPUT //
///////////

template<>
Packet& Packet::operator<< <Int8> (Int8 dt)
{
    m_data.push_back(static_cast<Byte>(Type::Int8));
    m_data.push_back(dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt8> (UInt8 dt)
{
    m_data.push_back(static_cast<Byte>(Type::UInt8));
    m_data.push_back(dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int16>(Int16 dt)
{
    m_data.push_back(static_cast<Byte>(Type::Int16));
    __push_16(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt16>(UInt16 dt)
{
    m_data.push_back(static_cast<Byte>(Type::UInt16));
    __push_16(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int32>(Int32 dt)
{
    m_data.push_back(static_cast<Byte>(Type::Int32));
    __push_32(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt32>(UInt32 dt)
{
    m_data.push_back(static_cast<Byte>(Type::UInt32));
    __push_32(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <Int64>(Int64 dt)
{
    m_data.push_back(static_cast<Byte>(Type::Int64));
    __push_64(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <UInt64>(UInt64 dt)
{
    m_data.push_back(static_cast<Byte>(Type::UInt64));
    __push_64(m_data, dt);
    return *this;
}

template<>
Packet& Packet::operator<< <float>(float dt)
{
    m_data.push_back(static_cast<Byte>(Type::Float));
    __push_32(m_data, static_cast<UInt32>(dt));
    return *this;
}

template<>
Packet& Packet::operator<< <double>(double dt)
{
    m_data.push_back(static_cast<Byte>(Type::Double));
    __push_64(m_data, static_cast<UInt64>(dt));
    return *this;
}

template<>
Packet& Packet::operator<< <const char*>(const char* dt)
{
    m_data.push_back(static_cast<Byte>(Type::String));
    while (*dt != 0)
        m_data.push_back(*dt++);
    m_data.push_back('\0');
    return *this;
}

template<>
Packet& Packet::operator<< <std::string>(std::string dt)
{
    return (*this) << dt.c_str();
}

////////////
// OUTPUT //
////////////

const ByteArray& Packet::get_data() const
{
    return m_data;
}

//
//Assert the packet isn't corrupted
//

#define __read(_fct, _type, _ptype, _errc)                              \
    {                                                                   \
        if (m_p.m_data.size() <= m_idx)                                 \
            return *this;                                               \
                                                                        \
        auto type = static_cast<Packet::Type>(m_p.m_data[m_idx]);       \
                                                                        \
        if (type != _ptype)                                             \
            throw PacketException(_errc);                               \
        m_idx++;                                                        \
                                                                        \
        dt = static_cast<_type>(_fct(m_idx, m_p.m_data));               \
        m_idx += sizeof(_type);                                         \
                                                                        \
        return *this;                                                   \
    }

static
inline
UInt8 __front_8(unsigned int index, const ByteArray& data)
{
    return static_cast<UInt8>(data[index]);
}

template<>
PacketReader& PacketReader::operator>> <UInt8> (UInt8& dt)
{
    __read(__front_8, UInt8, Packet::Type::UInt8,
           PacketExceptionT::UInt8Expected);
}

template<>
PacketReader& PacketReader::operator>> <Int8> (Int8& dt)
{
    __read(__front_8, Int8, Packet::Type::Int8,
           PacketExceptionT::Int8Expected);
}

template<>
PacketReader& PacketReader::operator>> <UInt16> (UInt16& dt)
{
    __read(__front_16, UInt16, Packet::Type::UInt16,
           PacketExceptionT::UInt16Expected);
}

template<>
PacketReader& PacketReader::operator>> <Int16> (Int16& dt)
{
    __read(__front_16, Int16, Packet::Type::Int16,
           PacketExceptionT::Int16Expected);
}

template<>
PacketReader& PacketReader::operator>> <UInt32> (UInt32& dt)
{
    __read(__front_32, UInt32, Packet::Type::UInt32,
           PacketExceptionT::UInt32Expected);
}

template<>
PacketReader& PacketReader::operator>> <Int32> (Int32& dt)
{
    __read(__front_32, Int32, Packet::Type::Int32,
           PacketExceptionT::Int32Expected);
}

template<>
PacketReader& PacketReader::operator>> <UInt64> (UInt64& dt)
{
    __read(__front_64, UInt64, Packet::Type::UInt64,
           PacketExceptionT::UInt64Expected);
}

template<>
PacketReader& PacketReader::operator>> <Int64> (Int64& dt)
{
    __read(__front_64, Int64, Packet::Type::Int64,
           PacketExceptionT::Int64Expected);
}

template<>
PacketReader& PacketReader::operator>> <float> (float& u_dt)
{
    UInt32 dt;
    __read(__front_32, UInt32, Packet::Type::Float,
           PacketExceptionT::FloatExpected);
    u_dt = static_cast<float>(dt);
}

template<>
PacketReader& PacketReader::operator>> <double> (double& u_dt)
{
    UInt64 dt;
    __read(__front_64, UInt64, Packet::Type::Double,
           PacketExceptionT::DoubleExpected);
    u_dt = static_cast<double>(dt);
}

template<>
PacketReader& PacketReader::operator>> <std::string> (std::string& dt)
{
    if (m_p.m_data.size() <= m_idx)
        return *this;

    auto type = static_cast<Packet::Type>(m_p.m_data[m_idx]);

    if (type != Packet::Type::String)
        throw PacketException(PacketExceptionT::StringExpected);
    m_idx++;

    std::string out;
    while (m_p.m_data[m_idx] != '\0')
        out.push_back(m_p.m_data[m_idx++]);

    using std::swap;
    swap(dt, out);

    return *this;
}

std::ostream& operator<< (std::ostream& os, const Packet& p)
{
    PacketReader r(p);

    os << "{ ";
    while (r)
    {
        switch(r.next_type())
        {
            //TODO Write it for each type
        case Packet::Type::Int32:
        {
            Int32 v;
            r >> v;
            os << v << " : " << "Int32, ";
        }
        case Packet::Type::String:
        {
            std::string v;
            r >> v;
            os << v << " : " << "String, ";
        }
        default:
            return os << "CORRUPTED }";
        }
    }
    return os << "}";
}

} // namespace SedNL
