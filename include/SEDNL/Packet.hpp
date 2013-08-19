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

////////////////////////////////////////////////////////////
//! \brief A packet.
////////////////////////////////////////////////////////////
class SEDNL_API Packet
{
public:

    //! \brief Low level binary representation of types.
    enum class Type
    {
        //! \brief Reserved type.
        Unknown = 0x00,
        //! \brief 8 bits signed integer.
        Int8    = 0x01,
        //! \brief 16 bits signed integer.
        Int16   = 0x02,
        //! \brief 32 bits signed integer.
        Int32   = 0x03,
        //! \brief 64 bits signed integer.
        Int64   = 0x04,
        //! \brief 8 bits unsigned integer.
        UInt8   = 0x05,
        //! \brief 16 bits unsigned integer.
        UInt16  = 0x06,
        //! \brief 32 bits unsigned integer.
        UInt32  = 0x07,
        //! \brief 64 bits unsigned integer.
        UInt64  = 0x08,
        //! \brief 32 bits floating number.
        Float   = 0x10,
        //! \brief 64 bits floating number.
        Double  = 0x11,
        //! \brief A C string (from a const char* or std::string::c_str())
        String  = 0x20,
        //! \brief An user defined type (serialized object).
        Object  = 0x40,
        // Different kind of arrays
        //! \brief An array of 8 bits signed integer.
        ArrayInt8    = 0x81,
        //! \brief An array of 16 bits signed integer.
        ArrayInt16   = 0x82,
        //! \brief An array of 32 bits signed integer.
        ArrayInt32   = 0x83,
        //! \brief An array of 64 bits signed integer.
        ArrayInt64   = 0x84,
        //! \brief An array of 8 bits unsigned integer.
        ArrayUInt8   = 0x85,
        //! \brief An array of 16 bits unsigned integer.
        ArrayUInt16  = 0x86,
        //! \brief An array of 32 bits unsigned integer.
        ArrayUInt32  = 0x87,
        //! \brief An array of 64 bits unsigned integer.
        ArrayUInt64  = 0x88,
        //! \brief An array of 32 bits floating number.
        ArrayFloat   = 0x90,
        //! \brief An array of 64 bits floating number.
        ArrayDouble  = 0x91,
    };

    //! Create an empty packet.
    Packet();

    //! Swap two packets.
    //!
    //! Swap the content of \a packet with the content of
    //! the current packet.
    //!
    //! \param[in,out] packet Packet to swap.
    inline
    void swap(Packet& packet) noexcept;

    //! \brief Write \a dt into the packet.
    //!
    //! If you call << on types like [unsigned] char, [unsigned] short,
    //! [unsigned] long, the matching [U]IntN operator will be called.
    //!
    //! In case of char, signed char will be stored (but it may change
    //! in futur implementations). Both unsigned char and signed char
    //! can be read as a char.
    //!
    //! In case of type = float, double
    //! the library assume that the system have 32, 64
    //! bits types. Otherwise, (bigger types) it may result in wrong memory access.
    //! You don't have to worry for most of the systems where
    //! this library would actualy compile.
    //!
    //! If this operation throw a std::bad_alloc, the packet is now in an invalid
    //! state and should't be used anymore.
    //!
    //! You can give it serialisable object (see Serializer.hpp).
    //!
    //! To read data from a packet, see PacketReader and PackerReader::operator>>().
    //!
    //! \param[in] dt Data
    //! \return A reference to this.
    template<typename T>
    Packet& operator<<(T dt);

    Packet& operator<< (const std::vector<char>& dt);

    Packet& operator<< (const std::vector<Int8>& dt);

    Packet& operator<< (const std::vector<Int16>& dt);

    Packet& operator<< (const std::vector<Int32>& dt);

    Packet& operator<< (const std::vector<Int64>& dt);

    Packet& operator<< (const std::vector<UInt8>& dt);

    Packet& operator<< (const std::vector<UInt16>& dt);

    Packet& operator<< (const std::vector<UInt32>& dt);

    Packet& operator<< (const std::vector<UInt64>& dt);

    Packet& operator<< (const std::vector<float>& dt);

    Packet& operator<< (const std::vector<double>& dt);

    //! \brief Return the computed data as a binary array.
    //!
    //! It's a reference to the data's internal buffer.
    //! It contains all writen data formated as Packet::Type followed
    //! by the value. Its what is send through the network.
    //!
    //! The handle become invalid when the Packet is destructed.
    //!
    //! \return Handle to internal buffer.
    const ByteArray& get_data() const;

    //! \brief Tell if it contains valid data.
    //!
    //! For implementation purpose.
    //!
    //! Do not call this function, all the packets you'll receive or
    //! send are valid.
    //!
    //! O(n) where n is the data length.
    //!
    //! \return True if the content is a valid packet, false otherwise.
    bool is_valid() noexcept;

    //! \brief Write all the argument as an object of length number_of_args(args).
    //!
    //! This produce the same behavior as serialising an object containing the
    //! values \a args with Packet::operator<<().
    template<typename... Args>
    friend
    void write_as_object(Packet& packet, Args&... args);

private:
    ByteArray m_data;

    //! \brief Write he header of an object (i.e. {Type::Object, length})
    //!        in the packet (without any data).
    void write_object_header(unsigned short length) throw(PacketException);

    //! \brief Try to validate one more item.
    bool valid_next_item(unsigned int size, unsigned int& i) noexcept;

    friend class PacketReader;
    friend class RingBuf;
};

////////////////////////////////////////////////////////////
//! \brief A reader (allow reading content from a Packet).
////////////////////////////////////////////////////////////
class SEDNL_API PacketReader
{
public:
    //! \brief Build a reader from a Packet \a p.
    inline PacketReader(const Packet& p);

    //! \brief Read \a dt from the PacketReader.
    //!
    //! This call is ignored if the end of the packet was reached.
    //! See PackerReader::operator bool().
    //!
    //! If you call >> on types like [unsigned] char, [unsigned] short,
    //! [unsigned] long, the matching [U]IntN operator will be called.
    //!
    //! In case of type = float, double
    //! the library assume that the system have 32, 64
    //! bits types. Otherwise, it will result in undefined behavior.
    //! You don't have to worry for most of the systems where
    //! this library would actualy compile.
    //!
    //! \param[out] dt Data
    //! \return A reference to this.
    template<typename T>
    PacketReader& operator>>(T &dt);

    PacketReader& operator>> (std::vector<char>& dt);

    PacketReader& operator>> (std::vector<Int8>& dt);

    PacketReader& operator>> (std::vector<Int16>& dt);

    PacketReader& operator>> (std::vector<Int32>& dt);

    PacketReader& operator>> (std::vector<Int64>& dt);

    PacketReader& operator>> (std::vector<UInt8>& dt);

    PacketReader& operator>> (std::vector<UInt16>& dt);

    PacketReader& operator>> (std::vector<UInt32>& dt);

    PacketReader& operator>> (std::vector<UInt64>& dt);

    PacketReader& operator>> (std::vector<float>& dt);

    PacketReader& operator>> (std::vector<double>& dt);


    //! \brief True until the packet was completely read.
    //!
    //! \return True if there is something to read, False otherwise.
    inline operator bool() const noexcept;

    //! \brief Return the type of the next element.
    //!
    //! \return The type of the next element to read. Unknow if the
    //!         end was reached.
    inline Packet::Type next_type() const noexcept;

private:
    //! \brief Consume the header of an object if it's a valid one (i.e. {Type::Object, length}).
    void read_object_header(unsigned short length) throw(PacketException);

    const Packet* m_p;
    unsigned int m_idx;

    //! \brief Write the next element of \a r into the stream \a os.
    bool output_one(int& i, std::ostream& os);

    template<typename... Args>
    friend void read_as_object(PacketReader& packet_reader, Args&... args);

    friend std::ostream& operator<< (std::ostream& os, const Packet& p);
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

//! \brief Write a short description of a packet in a JSON like fashion.
//!
//! The output is { data1 : type1, data2 : type2, ..., datan : typen, }.
//!
//! \param[out] os The output stream.
//! \param[in] p The packet to show.
//! \return The output stream.
SEDNL_API std::ostream& operator<< (std::ostream& os, const Packet& p);

//! \brief Read all the argument as an object of length number_of_args(args).
//!
//! Its the same behavior as unserialising a serialisable object containing
//! fields in the same order and same type as \a args.
//! See Serializer.hpp.
//!
//! \param[in] packet_reader Reader from wich to read data.
//! \param[out] args Variables to write into.
template<typename... Args>
void read_as_object(PacketReader& packet_reader, Args&... args);


//! \brief Allow creating easily new packets.
//!
//! You can create a packet with make_packet(arg1, arg2, ...).
//!
//! \param args Values to store in the new packet.
//! \return The newly created packet.
template<typename... Args>
Packet make_packet(Args... args);

//! \brief Allow writing easily into packets.
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
//! \param[out] args Variables in which data are stored.
template<typename... Args>
void read_from_packet(PacketReader& packet_reader, Args&... args);

//! \brief Convert a type to it's name as a string.
//!
//! \param[in] type The type to convert.
//! \return The type's name as a readable string.
SEDNL_API const char* type_to_string(Packet::Type type);

//! \brief Call type_to_string and output it.
//!
//! \param[out] os The output stream.
//! \param[in] type The type to show.
//! \return The output stream.
SEDNL_API std::ostream& operator<< (std::ostream& os, Packet::Type type);


//! \brief Give the number of arguments passed to it.
//!
//! Return the number of arguments given. For example,
//! number_of_args("str", 42, 0.1f) return 3.
//!
//! \return The number of arguments given to this function.
template<typename... Args>
inline
unsigned short number_of_args(Args... args);

//! Swap two packets.
//!
//! Swap the content of \a a with the content of
//! \a b.
//!
//! \param[in,out] a First Packet to swap.
//! \param[in,out] b Second Packet to swap.
inline
void swap(Packet& a, Packet& b) noexcept;

} // namespace SedNL

#include "SEDNL/Packet.ipp"

#endif /* !PACKET_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::Packet
//!
//! A Packet contain all the data you want to send / you received
//! as a binary string. The >> and << operator allow you to write to or
//! read from a packet.
//!
//! To allow reading multiple time the same packet, we use a PacketReader,
//! which is no more than an index telling where you are in the binary string.
//! That's why PacketReader are really cheap object with a strong API, allowing
//! type safe reading.
//!
//! Packet are used to build events, and events are what you are allowed to send
//! through a Connection object (TCPClient is a Connection).
//!
//! Here is some use cases.
//!
//! \code
//!
//! //Create an empty packet
//! Packet packet;
//!
//! //Write some stuff
//! packet << "Hello world";
//! packet << 42;
//! packet << 1.f << 2.f << 3.f << 4.f << 5.f;
//!
//! //You can also use this syntax :
//! write_to_packet(packet, 3.0, 2.0, 1.0);
//!
//! //You can even write array :
//! std::vector<int> ints = {1, 2, 3, 4, 5};
//!
//! packet << ints;
//!
//! //For serialised object, see Serializer.hpp
//!
//! //Then you can read from it.
//! PackerReader r1(packet);
//! PacketReader r2(packet);
//!
//! std::string str1, str2;
//! r1 >> str1;
//! r2 >> str2;
//! // both strings now contain "Hello world".
//!
//! int v;
//! r1 >> v; // v now contains 42
//!
//! //Read all 32 bits floating number, until encoutering something else.
//! while (r1.next_type() == Packet::Type::Float)
//! {
//!     float v;
//!     r1 >> v;
//!     std::cout << "32 bits floating number : " << v << std::endl;
//! }
//!
//! //Because readers do not modify packets, we can now
//! //build an event from packet.
//! Event event("my_stuff", packet);
//!
//! \endcode
////////////////////////////////////////////////////////////

