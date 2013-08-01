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

#ifndef PACKET_IPP_
#define PACKET_IPP_

namespace SedNL
{

inline
void Packet::swap(Packet& p) noexcept
{
    std::swap(m_data, p.m_data);
}

inline
void swap(Packet& a, Packet& b) noexcept
{
    a.swap(b);
}

PacketReader::PacketReader(const Packet &p)
    :m_p(&p), m_idx(0)
{}

template<typename T>
inline
PacketReader& operator>>(Packet &p, T &dt)
{
    return (PacketReader(p) >> (Int8&)dt);
}

PacketReader::operator bool() const noexcept
{
    return !(m_idx >= m_p->m_data.size());
}

Packet::Type PacketReader::next_type() const noexcept
{
    return static_cast<Packet::Type>(m_p->m_data[m_idx]);
}

template<typename... Args>
Packet make_packet(Args... args)
{
    Packet p;
    write_to_packet(p, args...);
    return p;
}

template<typename T, typename... Args>
void write_to_packet(Packet& p, T arg, Args... args)
{
    write_to_packet(p << arg, args...);
}

inline void write_to_packet(Packet&)
{}

template<typename T, typename... Args>
void read_from_packet(PacketReader& p, T& arg, Args&... args)
{
    read_from_packet(p >> arg, args...);
}

inline void read_from_packet(PacketReader&)
{}

inline
unsigned short number_of_args_aux()
{
    return 0;
}

template<typename T, typename... Args>
inline
unsigned short number_of_args_aux(T, Args... args)
{
    return 1 + number_of_args_aux(args...);
}

template<typename... Args>
inline
unsigned short number_of_args(Args... args)
{
    return number_of_args_aux(args...);
}

template<typename... Args>
void write_as_object(Packet& packet, Args&... args)
{
    const unsigned short nb_args = number_of_args(args...);

    packet.write_object_header(nb_args);
    write_to_packet(packet, args...);
}

//! \brief Read all the argument as an object of length number_of_args(args)
template<typename... Args>
void read_as_object(PacketReader& packet_reader, Args&... args)
{
    const unsigned short nb_args = number_of_args(args...);

    PacketReader tmp_reader = packet_reader;
    tmp_reader.read_object_header(nb_args);
    read_from_packet(tmp_reader, args...);
    //If everything gone well, just modify the reader
    using std::swap;
    swap(packet_reader, tmp_reader);
}

} // namespace SedNL

namespace std
{
    template<>
    inline
    void swap(SedNL::Packet& lhs, SedNL::Packet& rhs) noexcept
    {
        SedNL::swap(lhs, rhs);
    }
}

#endif /* !PACKET_IPP_ */
