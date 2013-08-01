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
#include "SEDNL/Exception.hpp"
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

    //! Swap two packets
    //!
    //! \param[in,out] packet
    inline
    void swap(Packet& packet) noexcept;

    //! \brief Write \a dt into the packet
    //!
    //! If you call << on types like [unsigned] char, [unsigned] short,
    //! [unsigned] long, the matching [U]IntN operator will be called.
    //!
    //! In case of type = float, double
    //! the library assume that the system have 32, 64
    //! bits types. Otherwise, they will be casted to their UIntN
    //! conterpart. You don't have to worry for most of the systems where
    //! this library would actualy compile.
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

    //! \brief Tell if it contains valid data
    //!
    //! For implementation purpose. Do not call this function,
    //! all the packets you'll see are valid.
    //!
    //! O(n) where n is the data length.
    //!
    //! \return True if the content is a valid packet, false otherwise.
    bool is_valid() noexcept;


    //! \brief Write all the argument as an object of length number_of_args(args)
    template<typename... Args>
    friend
    void write_as_object(Packet& packet, Args&... args);

private:
    ByteArray m_data;

    //Write only the header of an object (i.e. {Type::Object, length})
    void write_object_header(unsigned short length) throw(PacketException);

    friend class PacketReader;
    friend class RingBuf;
};

class SEDNL_API PacketReader
{
public:
    //! \brief Build a reader from a packet
    inline PacketReader(const Packet& p);

    //! \brief Read \a dt from the packet
    //!
    //! If you call >> on types like [unsigned] char, [unsigned] short,
    //! [unsigned] long, the matching [U]IntN operator will be called.
    //!
    //! In case of type = float, double
    //! the library assume that the system have 32, 64
    //! bits types. Otherwise, they will be casted to their UIntN
    //! conterpart. You don't have to worry for most of the systems where
    //! this library would actualy compile.
    //!
    //! \param[out] dt Data
    template<typename T>
    PacketReader& operator>>(T &dt);

    //! \brief True until the packet was completely read
    inline operator bool() const noexcept;

    //! \brief Return the type of the next element
    inline Packet::Type next_type() const noexcept;

    //! \brief Read all the argument as an object of length number_of_args(args)
    template<typename... Args>
    friend
    void read_as_object(PacketReader& packet_reader, Args&... args);

private:
    //! \brief Consume the header of an object if it's a valid one (i.e. {Type::Object, length})
    void read_object_header(unsigned short length) throw(PacketException);

    const Packet* m_p;
    unsigned int m_idx;
};

//IN
template<>
Packet& Packet::operator<< <char>(char dt);
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
Packet& Packet::operator<< <float>(float dt);
template<>
Packet& Packet::operator<< <double>(double dt);

template<>
Packet& Packet::operator<< <std::string>(std::string dt);
template<>
Packet& Packet::operator<< <const char*>(const char* dt);

//OUT
template<>
PacketReader& PacketReader::operator>> <char>(char& dt);
template<>
PacketReader& PacketReader::operator>> <Int8>(Int8& dt);
template<>
PacketReader& PacketReader::operator>> <Int16>(Int16& dt);
template<>
PacketReader& PacketReader::operator>> <Int32>(Int32& dt);
template<>
PacketReader& PacketReader::operator>> <Int64>(Int64& dt);
template<>
PacketReader& PacketReader::operator>> <UInt8>(UInt8& dt);
template<>
PacketReader& PacketReader::operator>> <UInt16>(UInt16& dt);
template<>
PacketReader& PacketReader::operator>> <UInt32>(UInt32& dt);
template<>
PacketReader& PacketReader::operator>> <UInt64>(UInt64& dt);

template<>
PacketReader& PacketReader::operator>> <float>(float& dt);
template<>
PacketReader& PacketReader::operator>> <double>(double& dt);

template<>
PacketReader& PacketReader::operator>> <std::string>(std::string& dt);

template<typename T>
inline
PacketReader& operator>> (Packet &p, T &dt);

//! \brief Write a short description of a packet in a JSON like fashion
//!
//! The output is { data1 : type1, data2 : type2, ..., datan : typen, }.
//!
//! \param[out] os The output stream
//! \param[in] p The packet to describe
//! \return The output stream
SEDNL_API std::ostream& operator<< (std::ostream& os, const Packet& p);

//! \brief Allow creating easily packets
//!
//! You can create a packet with make_packet(arg1, arg2, ...)
//!
//! \return The newly created packet
template<typename... Args>
Packet make_packet(Args... args);

//! \brief Allow writing easily into packets
//!
//! You can write into a packet with write_to_packet(packet, arg1, arg2, ...)
//!
//! \param[out] packet Pacet in which data are stored.
//! \param[in] args Data to write sequencialy into the packet.
template<typename... Args>
void write_to_packet(Packet& packet, Args... args);

//! \brief Allow reading data from a PacketReader.
//!
//! You can read a packet with read_from_packet(packet_reader, arg1, arg2, ...)
//!
//! \param[in] packet_reader Stream to read data from.
//! \param[out] args Variable in which data are stored.
template<typename... Args>
void read_from_packet(PacketReader& packet_reader, Args&... args);

//! \brief Convert a type to it's name as a string
//!
//! \param[in] type The type to convert
//! \return The type's name as a readable string
SEDNL_API const char* type_to_string(Packet::Type type);

//! \brief Call type_to_string and output it
SEDNL_API std::ostream& operator<< (std::ostream& os, Packet::Type type);


//! \brief Give the number of arguments passed to it.
template<typename... Args>
inline
unsigned short number_of_args(Args... args);

} // namespace SedNL

#include "SEDNL/Packet.ipp"

#endif /* !PACKET_HPP_ */
