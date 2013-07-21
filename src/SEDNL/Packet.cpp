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

//TODO : Implement all get operator
//     : Use a deque or an internal counter
//       instead of removing elements of the vector
//       (the cost is hight and should be avoided).

template<>
Packet& Packet::operator>> <Int8> (Int8& dt)
{
    auto type = static_cast<Type>(m_data.front());
    if (type != Type::Int8)
        throw TypeException(TypeExceptionT::WrongPacketType);

    m_data.erase(m_data.begin());
    dt = static_cast<Int8>(m_data.front());
    m_data.erase(m_data.begin());
    return *this;
}


const ByteArray& Packet::get_data() const
{
    return m_data;
}

} // namespace SedNL
