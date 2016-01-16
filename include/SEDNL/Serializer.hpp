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

#ifndef SERIALIZER_HPP_
#define SERIALIZER_HPP_

#include <string>

#include "SEDNL/Packet.hpp"

namespace SedNL
{

#define SEDNL_SERIALIZABLE(...)                                      \
    inline void serialize(Packet& p)                                 \
    {                                                                \
        serializer_serialize(p, *this, __VA_ARGS__);                 \
    }                                                                \
    inline void unserialize(PacketReader& p)                         \
    {                                                                \
        serializer_unserialize(p, *this, __VA_ARGS__);               \
    }                                                                \
    inline void unserialize(const Packet& p)                         \
    { PacketReader r(p); unserialize(r); };

//! Implementation of serialization.
template<typename T, typename... Args>
void serializer_serialize(Packet& packet, T&inst, Args&... args);

//! Implementation of unserialization.
template<typename T, typename... Args>
void serializer_unserialize(PacketReader& packet_reader, T&inst, Args&... args);

};

#include "Serializer.ipp"

#endif /* !SERIALIZER_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \file Serializer.hpp
//! \brief Allow serialization of instances.
//!
//! This file provide the macro and function definition needed
//! to make your instances serialisable.
//!
//! Basicaly, any class implementing void serialize(Packet& p)
//! and void unserialize(PacketReader& p) is unserialisable.
//!
//! The implementation provided by the macro SEDNL_SERIALIZABLE(...)
//! simply call serializer_serialize(packet, *this, members...)
//! and serializer_unserialize(packet_reader, *this, members...).
//!
//! This means that if you don't like macro, you can wrote it
//! by hand (but it's a bad idea, because the macro give you consistency :
//! Using the macro, the unserialise function will always exactly work on
//! the same members than the unserialise function).
//!
//! If you want custom processing before / after serialisation / unserialisation
//! you can implement some of the 4 folowing member functions :
//!
//! \code
//!
//! void before_serialization();
//! void after_serialization();
//! void before_unserialization();
//! void after_unserialization();
//!
//! \endcode
//!
//! As you can guess, they will be called before / after the serialization
//! / unserialization process. Thats the recomanded way for costumisation
//! of serialization and unserialization process.
//!
//! A serializable instances (i.e. an instance of a serializable class)
//! can be used with Packet::operator<<() and PacketReader::operator>>().
//! It means that you can read / write it as if it was a simple string.
//!
//! If you want to read / write a bunch of data as a serialized object
//! without actualy implementing a serializable class (for debug purpose, simple
//! project where one of the two side don't need a class, etc...) you can
//! use the convenience functions write_as_object(Packet& packet, Args&...) and
//! read_as_object(PacketReader&, Args&...).
//!
//! Here is a short example :
//!
//! \code
//! class Foo
//! {
//! public:
//!     Foo()
//!         :m_int(42), m_char(23), m_string("str")
//!     {};
//!
//!     void before_serialization()
//!     {
//!         std::cout << "Before serialization" << std::endl;
//!     };
//!
//!     void after_serialization()
//!     {
//!         std::cout << "After serialization" << std::endl;
//!     };
//!
//!     void before_unserialization()
//!     {
//!         std::cout << "Before unserialization" << std::endl;
//!     };
//!
//!     void after_unserialization()
//!     {
//!         std::cout << "After unserialization" << std::endl;
//!     };
//!
//!     SEDNL_SERIALIZABLE(m_int, m_char, m_string);
//!
//! private:
//!     int m_int;
//!     char m_char;
//!     std::string m_string;
//! };
//!
//! //Create a Foo instance
//! Foo first;
//!
//! //Serialize the Foo instance
//! Packet p;
//! p << first; //same as first.serialize(p);
//!
//! //Unserialize into the a instance
//! Foo second;
//! p >> second; //same as second.unserialize(p);
//! \endcode
//!
//!
//!
//! \def SEDNL_SERIALIZABLE(...)
//! \brief Allow objects to be serialized.
//!
//! To allow one of you class to be serialisable, you can
//! call the SEDNL_SERIALIZE() inside your class definition
//! with the list of fields you would like to see serialized.
//!
//! This will create two member methods serialize and unserialize.
//!
//! Then, you can call your_instance.serialize(packet)
//! and your_instance.unserialize(packet_reader).
//!
//! A simple example of use is
//!
//! \code
//!
//! class MyClass
//! {
//! public:
//!     SEDNL_SERIALIZABLE(m_a, m_b);
//! private:
//!     int m_a;
//!     int m_b;
//! };
//!
//! \endcode
//!
//! Here is the code generated by a call to this macro :
//!
//! \code
//!
//! inline void serialize(Packet& p)
//! {
//!     serializer_serialize(p, *this, __VA_ARGS__);
//! }
//! inline void unserialize(PacketReader& p)
//! {
//!     serializer_unserialize(p, *this, __VA_ARGS__);
//! }
//! inline void unserialize(const Packet& p)
//! {
//!     PacketReader r(p);
//!     unserialize(r);
//! };
//!
//! \endcode
//!
////////////////////////////////////////////////////////////
