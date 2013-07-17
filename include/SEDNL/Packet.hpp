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

#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/Types.hpp"

#include <vector>

namespace SedNL
{

typedef std::vector<Byte> ByteArray;

class RingBuf;

class SEDNL_API Packet
{
public:

    //! \brief Low level binary representation of types
    enum class Type
    {
        //! Reserved type
        Unknown = 0,
        Int8    = 1,
        Int16   = 2,
        Int32   = 3,
        Int64   = 4,
        UInt8   = 5,
        UInt16  = 6,
        UInt32  = 7,
        UInt64  = 8,
        Float   = 20,
        Double  = 21,
        //! \brief A C string (from a const char* or std::string)
        String  = 30,
        //! \brief An user defined type
        Object  = 40,
    };

    //! Create an empty packet
    Packet();

    //! \brief Write \a dt into the packet
    //!
    //! If you call << on types like [unsigned] char, [unsigned] short,
    //! [unsigned] long, the matching [U]IntN operator will be called.
    //!
    //! In case of type = float, double
    //! the library assume that the system have 32, 64
    //! bits types. Otherwise, they will be casted to their UIntN
    //! conterpart. You don't have to worry for most of the systems where
    //! this library would actualy works.
    //!
    //! \param[in] dt Data
    template<typename T>
    Packet& operator<<(T dt);

    //! \brief Return the computed data
    //!
    //! It's a reference to the data's internal buffer.
    //! This object is destructed when the Packet is destructed.
    //!
    //! \return Internal buffer
    const ByteArray& get_data() const;

    //! \brief Return the data, in network byte order
    //!
    //! The length of data from get_data(), but in network
    //! byte order.
    //!
    //! This is a really low level function and you probably won't
    //! have to use it.
    //!
    //! \return Internal buffer
    UInt16 get_network_length() const;

private:
    ByteArray data;

    friend RingBuf;
};

template<>
Packet& Packet::operator<< <Int8>(Int8 dt);
template<>
Packet& Packet::operator<< <Int16>(Int16 dt);
template<>
Packet& Packet::operator<< <Int32>(Int32 dt);
template<>
Packet& Packet::operator<< <Int64>(Int64 dt);
template<>
Packet& Packet::operator<< <UInt8>(UInt8 dt);
template<>
Packet& Packet::operator<< <UInt16>(UInt16 dt);
template<>
Packet& Packet::operator<< <UInt32>(UInt32 dt);
template<>
Packet& Packet::operator<< <UInt64>(UInt64 dt);

template<>
Packet& Packet::operator<< <std::string>(std::string dt);
template<>
Packet& Packet::operator<< <const char*>(const char* dt);


} // namespace SedNL

#endif /* !PACKET_HPP_ */
