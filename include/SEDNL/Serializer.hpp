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

    class Packet;
    class PacketReader;
    template <class T>
    struct Serializer;

//! \brief Allow object to be serialized
//!
//! To allow one of you class to be serialisable, you can
//! call the SEDNL_SERIALIZE() inside your class definition
//! with the list of fields you would like to see serialized.
//!
//! This will create two member methods serialize and unserialize.
//!
//! Then, you can call your_instance.serialize(packet)
//! and your_instance.unserialize(packet_reader).
#define SEDNL_SERIALIZABLE(type, ...)                                \
        void serialize(Packet& p)                                    \
        {                                                            \
            SedNL::Serializer<type>::pre_serialize(*this, 0);        \
            write_to_packet(p, __VA_ARGS__);                         \
            SedNL::Serializer<type>::post_serialize(*this, 0);       \
        }                                                            \
        void unserialize(PacketReader& p)                            \
        {                                                            \
            SedNL::Serializer<type>::pre_unserialize(*this, 0);      \
            read_from_packet(p, __VA_ARGS__);                        \
            SedNL::Serializer<type>::post_unserialize(*this, 0);     \
        }                                                            \
        void unserialize(const Packet& p)                            \
        { PacketReader r(p); unserialize(r); };

};

#include "Serializer.ipp"

#endif /* !SERIALIZER_HPP_ */
