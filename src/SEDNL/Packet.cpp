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

#include <cassert>

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
        //TODO : Object and Array
        case Type::Object:
        case Type::ArrayInt8:
        case Type::ArrayInt16:
        case Type::ArrayInt32:
        case Type::ArrayInt64:
        case Type::ArrayUInt8:
        case Type::ArrayUInt16:
        case Type::ArrayUInt32:
        case Type::ArrayUInt64:
        case Type::ArrayFloat:
        case Type::ArrayDouble:
            return true;
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
Packet& Packet::operator<< <char> (char dt)
{
    m_data.push_back(static_cast<Byte>(Type::Int8));
    m_data.push_back(dt);
    return *this;
}

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
    __push_32(m_data, reinterpret_cast<UInt32&>(dt));
    return *this;
}

template<>
Packet& Packet::operator<< <double>(double dt)
{
    m_data.push_back(static_cast<Byte>(Type::Double));
    __push_64(m_data, reinterpret_cast<UInt64&>(dt));
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

#define __array(fct, cast_type, type)                           \
    if (static_cast<UInt16>(dt.size()) != dt.size())            \
        throw PacketException(PacketExceptionT::WrongArray);    \
                                                                \
    m_data.push_back(static_cast<Byte>(type));                  \
    __push_16(m_data, static_cast<UInt16>(dt.size()));          \
                                                                \
    for (auto elm : dt)                                         \
        fct(m_data, reinterpret_cast<cast_type&>(elm));         \

template<typename T, typename S>
inline
void __push_8(std::vector<T>& data, S elm)
{
    data.push_back(static_cast<T>(elm));
}

Packet& Packet::operator<< (const std::vector<char>& dt)
{
    __array(__push_8, Int8, Type::ArrayInt8);
    return *this;
}

Packet& Packet::operator<< (const std::vector<Int8>& dt)
{
    __array(__push_8, Int8, Type::ArrayInt8);
    return *this;
}

Packet& Packet::operator<< (const std::vector<UInt8>& dt)
{
    __array(__push_8, UInt8, Type::ArrayUInt8);
    return *this;
}

Packet& Packet::operator<< (const std::vector<Int16>& dt)
{
    __array(__push_16, Int16, Type::ArrayInt16);
    return *this;
}

Packet& Packet::operator<< (const std::vector<UInt16>& dt)
{
    __array(__push_16, UInt16, Type::ArrayUInt16);
    return *this;
}

Packet& Packet::operator<< (const std::vector<Int32>& dt)
{
    __array(__push_32, Int32, Type::ArrayInt32);
    return *this;
}

Packet& Packet::operator<< (const std::vector<UInt32>& dt)
{
    __array(__push_32, UInt32, Type::ArrayUInt32);
    return *this;
}

Packet& Packet::operator<< (const std::vector<Int64>& dt)
{
    __array(__push_64, Int64, Type::ArrayInt64);
    return *this;
}

Packet& Packet::operator<< (const std::vector<UInt64>& dt)
{
    __array(__push_64, UInt64, Type::ArrayUInt64);
    return *this;
}

Packet& Packet::operator<< (const std::vector<float>& dt)
{
    __array(__push_32, UInt32, Type::ArrayFloat);
    return *this;
}

Packet& Packet::operator<< (const std::vector<double>& dt)
{
    __array(__push_64, UInt64, Type::ArrayDouble);
    return *this;
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

#define __read(_fct, _type, _ptype)                                     \
    {                                                                   \
        if (m_p->m_data.size() <= m_idx)                                \
            return *this;                                               \
                                                                        \
        auto type = static_cast<Packet::Type>(m_p->m_data[m_idx]);      \
                                                                        \
        if (type != _ptype)                                             \
            exception_by_type(type);                                    \
        m_idx++;                                                        \
                                                                        \
        auto dt_ = _fct(m_idx, m_p->m_data);                            \
        dt = reinterpret_cast<_type&>(dt_);                             \
        m_idx += sizeof(_type);                                         \
                                                                        \
        return *this;                                                   \
    }

static
void exception_by_type(Packet::Type type)
{
    switch(type)
    {
    case  Packet::Type::Int8:
        throw PacketException(PacketExceptionT::Int8Expected);
    case Packet::Type::Int16:
        throw PacketException(PacketExceptionT::Int16Expected);
    case Packet::Type::Int32:
        throw PacketException(PacketExceptionT::Int32Expected);
    case Packet::Type::Int64:
        throw PacketException(PacketExceptionT::Int64Expected);

    case Packet::Type::UInt8:
        throw PacketException(PacketExceptionT::UInt8Expected);
    case Packet::Type::UInt16:
        throw PacketException(PacketExceptionT::UInt16Expected);
    case Packet::Type::UInt32:
        throw PacketException(PacketExceptionT::UInt32Expected);
    case Packet::Type::UInt64:
        throw PacketException(PacketExceptionT::UInt64Expected);

    case Packet::Type::Float:
        throw PacketException(PacketExceptionT::FloatExpected);
    case Packet::Type::Double:
        throw PacketException(PacketExceptionT::DoubleExpected);

    case Packet::Type::ArrayInt8:
        throw PacketException(PacketExceptionT::ArrayInt8Expected);
    case Packet::Type::ArrayInt16:
        throw PacketException(PacketExceptionT::ArrayInt16Expected);
    case Packet::Type::ArrayInt32:
        throw PacketException(PacketExceptionT::ArrayInt32Expected);
    case Packet::Type::ArrayInt64:
        throw PacketException(PacketExceptionT::ArrayInt64Expected);

    case Packet::Type::ArrayUInt8:
        throw PacketException(PacketExceptionT::ArrayUInt8Expected);
    case Packet::Type::ArrayUInt16:
        throw PacketException(PacketExceptionT::ArrayUInt16Expected);
    case Packet::Type::ArrayUInt32:
        throw PacketException(PacketExceptionT::ArrayUInt32Expected);
    case Packet::Type::ArrayUInt64:
        throw PacketException(PacketExceptionT::ArrayUInt64Expected);

    case Packet::Type::ArrayFloat:
        throw PacketException(PacketExceptionT::ArrayFloatExpected);
    case Packet::Type::ArrayDouble:
        throw PacketException(PacketExceptionT::ArrayDoubleExpected);

    case Packet::Type::String:
        throw PacketException(PacketExceptionT::StringExpected);

    case Packet::Type::Object:
        throw PacketException(PacketExceptionT::ObjectExpected);

    default:
        std::cout << ":" << (int)(unsigned char)type <<" " << (int)(unsigned char)Packet::Type::ArrayInt8 << " " << (int)(type == Packet::Type::ArrayInt8) << std::endl;
        throw PacketException(PacketExceptionT::Unknown);
    }
}

static
inline
UInt8 __front_8(unsigned int index, const ByteArray& data)
{
    return static_cast<UInt8>(data[index]);
}

template<>
PacketReader& PacketReader::operator>> <char> (char& dt)
{
    if (m_p->m_data.size() <= m_idx)
        return *this;

    auto type = static_cast<Packet::Type>(m_p->m_data[m_idx]);

    if (type != Packet::Type::Int8 && type != Packet::Type::UInt8)
        exception_by_type(type);
    m_idx++;

    dt = static_cast<char>(__front_8(m_idx, m_p->m_data));
    m_idx += sizeof(Int8);

    return *this;
}

template<>
PacketReader& PacketReader::operator>> <UInt8> (UInt8& dt)
{
    __read(__front_8, UInt8, Packet::Type::UInt8);
}

template<>
PacketReader& PacketReader::operator>> <Int8> (Int8& dt)
{
    __read(__front_8, Int8, Packet::Type::Int8);
}

template<>
PacketReader& PacketReader::operator>> <UInt16> (UInt16& dt)
{
    __read(__front_16, UInt16, Packet::Type::UInt16);
}

template<>
PacketReader& PacketReader::operator>> <Int16> (Int16& dt)
{
    __read(__front_16, Int16, Packet::Type::Int16);
}

template<>
PacketReader& PacketReader::operator>> <UInt32> (UInt32& dt)
{
    __read(__front_32, UInt32, Packet::Type::UInt32);
}

template<>
PacketReader& PacketReader::operator>> <Int32> (Int32& dt)
{
    __read(__front_32, Int32, Packet::Type::Int32);
}

template<>
PacketReader& PacketReader::operator>> <UInt64> (UInt64& dt)
{
    __read(__front_64, UInt64, Packet::Type::UInt64);
}

template<>
PacketReader& PacketReader::operator>> <Int64> (Int64& dt)
{
    __read(__front_64, Int64, Packet::Type::Int64);
}

template<>
PacketReader& PacketReader::operator>> <float> (float& dt)
{
    __read(__front_32, float, Packet::Type::Float);
}

template<>
PacketReader& PacketReader::operator>> <double> (double& dt)
{
    __read(__front_64, double, Packet::Type::Double);
}

template<>
PacketReader& PacketReader::operator>> <std::string> (std::string& dt)
{
    if (m_p->m_data.size() <= m_idx)
        return *this;

    auto type = static_cast<Packet::Type>(m_p->m_data[m_idx]);

    if (type != Packet::Type::String)
        throw PacketException(PacketExceptionT::StringExpected);
    m_idx++;

    std::string out;
    while (m_p->m_data[m_idx] != '\0')
        out.push_back(m_p->m_data[m_idx++]);
    m_idx++;

    using std::swap;
    swap(dt, out);

    return *this;
}

#define __read_array(fct, cast_type, packet_type)                       \
    if (m_p->m_data.size() <= m_idx)                                    \
        return *this;                                                   \
                                                                        \
    auto type = static_cast<Packet::Type>(m_p->m_data[m_idx]);          \
                                                                        \
    if (type != packet_type)                                            \
        exception_by_type(type);                                        \
    m_idx++;                                                            \
                                                                        \
    unsigned short length = __front_16(m_idx, m_p->m_data);             \
    m_idx += sizeof(UInt16);                                            \
                                                                        \
    dt.clear();                                                         \
    for (int i = 0; i < length; i++)                                    \
    {                                                                   \
        auto t = fct(m_idx, m_p->m_data);                               \
        dt.push_back(reinterpret_cast<cast_type&>(t));                  \
        m_idx += sizeof(cast_type);                                     \
    }                                                                   \
                                                                        \
    return *this;


PacketReader& PacketReader::operator>> (std::vector<char>& dt)
{
    if (m_p->m_data.size() <= m_idx)
        return *this;

    auto type = static_cast<Packet::Type>(m_p->m_data[m_idx]);

    if (type != Packet::Type::ArrayInt8 && type != Packet::Type::ArrayUInt8)
        exception_by_type(type);
    m_idx++;

    unsigned short length = __front_16(m_idx, m_p->m_data);
    m_idx += sizeof(UInt16);

    dt.clear();
    for (int i = 0; i < length; i++)
    {
        dt.push_back(static_cast<char>(m_p->m_data[m_idx]));
        m_idx += sizeof(UInt8);
    }

    return *this;
}

PacketReader& PacketReader::operator>> (std::vector<UInt8>& dt)
{
    __read_array(__front_8, UInt8, Packet::Type::ArrayUInt8);
}

PacketReader& PacketReader::operator>> (std::vector<Int8>& dt)
{
    __read_array(__front_8, Int8, Packet::Type::ArrayInt8);
}

PacketReader& PacketReader::operator>> (std::vector<UInt16>& dt)
{
    __read_array(__front_16, UInt16, Packet::Type::ArrayUInt16);
}

PacketReader& PacketReader::operator>> (std::vector<Int16>& dt)
{
    __read_array(__front_16, Int16, Packet::Type::ArrayInt16);
}

PacketReader& PacketReader::operator>> (std::vector<UInt32>& dt)
{
    __read_array(__front_32, UInt32, Packet::Type::ArrayUInt32);
}

PacketReader& PacketReader::operator>> (std::vector<Int32>& dt)
{
    __read_array(__front_32, Int32, Packet::Type::ArrayInt32);
}

PacketReader& PacketReader::operator>> (std::vector<UInt64>& dt)
{
    __read_array(__front_64, UInt64, Packet::Type::ArrayUInt64);
}

PacketReader& PacketReader::operator>> (std::vector<Int64>& dt)
{
    __read_array(__front_64, Int64, Packet::Type::ArrayInt64);
}

PacketReader& PacketReader::operator>> (std::vector<float>& dt)
{
    __read_array(__front_32, float, Packet::Type::ArrayFloat);
}

PacketReader& PacketReader::operator>> (std::vector<double>& dt)
{
    __read_array(__front_64, double, Packet::Type::ArrayDouble);
}

std::ostream& operator<< (std::ostream& os, const Packet& p)
{
    PacketReader r(p);
    Packet::Type type;

#define __show_data_case(type) {type v; r >> v; os << v; break;}

    os << "{ ";
    while (r)
    {
        switch(type = r.next_type())
        {
        case Packet::Type::Int8:
            __show_data_case(Int8);
        case Packet::Type::Int16:
            __show_data_case(Int16);
        case Packet::Type::Int32:
            __show_data_case(Int32);
        case Packet::Type::Int64:
            __show_data_case(Int64);

        case Packet::Type::UInt8:
            __show_data_case(UInt8);
        case Packet::Type::UInt16:
            __show_data_case(UInt16);
        case Packet::Type::UInt32:
            __show_data_case(UInt32);
        case Packet::Type::UInt64:
            __show_data_case(UInt64);

        case Packet::Type::Float:
            __show_data_case(float);
        case Packet::Type::Double:
            __show_data_case(double);

        case Packet::Type::String:
            __show_data_case(std::string);

        case Packet::Type::Object:
            //TODO : handle the case of objects
            return os << "OBJECT }";

        default:
            return os << "CORRUPTED }";
        }
        os << " : " << type_to_string(type) << ", ";
    }
    return os << "}";
}

const char* type_to_string(Packet::Type type)
{
    switch(type)
    {
    case Packet::Type::Int8:
        return "Int8";
    case Packet::Type::Int16:
        return "Int16";
    case Packet::Type::Int32:
        return "Int32";
    case Packet::Type::Int64:
        return "Int64";
    case Packet::Type::UInt8:
        return "UInt8";
    case Packet::Type::UInt16:
        return "UInt16";
    case Packet::Type::UInt32:
        return "UInt32";
    case Packet::Type::UInt64:
        return "UInt64";
    case Packet::Type::Float:
        return "Float";
    case Packet::Type::Double:
        return "Double";
    case Packet::Type::String:
        return "String";
    case Packet::Type::Object:
        return "Object";
    case Packet::Type::Unknown:
    default:
        return "Unknown";
    }
}

std::ostream& operator<< (std::ostream& os, Packet::Type type)
{
    return os << type_to_string(type);
}

void PacketReader::read_object_header(unsigned short length)
    throw(PacketException)
{
    Packet::Type type = next_type();
    if (type != Packet::Type::Object)
        throw PacketException(PacketExceptionT::ObjectExpected);
    UInt8 obj_length = static_cast<UInt8>(m_p->m_data[m_idx + 1]);
    if (obj_length == 0)
        throw PacketException(PacketExceptionT::EmptyObject);
    if (obj_length != length)
        throw PacketException(PacketExceptionT::WrongSizedObject);

    //Ok, just skip the header
    m_idx += 2;
}

void Packet::write_object_header(unsigned short length)
    throw(PacketException)
{
    assert(length < 256);
    if (length == 0)
        throw PacketException(PacketExceptionT::EmptyObject);
    m_data.push_back(static_cast<UInt8>(Packet::Type::Object));
    m_data.push_back(static_cast<UInt8>(length));
}

} // namespace SedNL
