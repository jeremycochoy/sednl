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

#include "SEDNL/RingBuf.hpp"
#include "SEDNL/SocketHelp.hpp"
#include "SEDNL/Event.hpp"

#define ROUND(pos) ((pos) % (m_size + 1))
#define AT(pos)    m_dt[(pos)]

namespace SedNL
{

// We choose to 'lost' one byte, so that a full buffer contain exactly size bytes,
// and we can read data in [m_start, m_end] (both included).
RingBuf::RingBuf(unsigned int size) throw(std::bad_alloc)
    :m_dt(new UInt8[size + 1]), m_size(size), m_start(0), m_end(0)
{}

bool RingBuf::put(const char* string, unsigned int length) noexcept
{
    //Not enougth memory
    if (RingBuf::length() + length > m_size)
        return false;

    //Let's write it
    while(length--)
    {
        m_dt.get()[m_end] = *string;
        string++;
        m_end = (m_end + 1) % (m_size + 1);
    }

    return true;
}

bool RingBuf::pick_event(Event& event) noexcept
{
    std::cout << "RINGBUFFER-- PICK" <<  m_start << " " << m_end<<std::endl;
    try
    {
        //Event header start by an UInt16 wich is the packet length.
        // So, we need at least the packet size
        if (length() < sizeof(UInt16))
            return false;

        UInt16 packet_length;
        UInt8* ptr = reinterpret_cast<UInt8*>(&packet_length);
        ptr[0] = AT(m_start);
        ptr[1] = AT(m_start + 1);
        packet_length = ntohs(packet_length);

        //We want : UInt16 + '\0' terminated string, so at least
        // sizeof(UInt16) + sizeof(UInt8)
        if (packet_length < sizeof(UInt16) + sizeof(UInt8))
            packet_length = sizeof(UInt16) + sizeof(UInt8);

        //We also want the whole packed
        if (length() < packet_length)
            return false;

        //Begining of the event name
        unsigned int dt_idx = ROUND(m_start + sizeof(UInt16));
        unsigned int remaining = packet_length - sizeof(UInt16);
        std::string name;

#define NEXT_BYTE() {remaining--;dt_idx++;}

        while (remaining && AT(dt_idx) != '\0')
        {
            name.push_back(AT(dt_idx));
            NEXT_BYTE();
        }
        if (AT(dt_idx) != '\0')
        {
            //It's a corrupted packet.
            //Log it and drop it
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: Corrupted packet. Droped." << std::endl;
#endif /* !SEDNL_NOWARN */
            m_start = ROUND(m_start + packet_length);
            return false;
        }
        Packet packet;

        //Jump over the '\0'
        NEXT_BYTE();
        //Read packet content
        while (remaining)
        {
            packet.m_data.push_back(AT(dt_idx));
            NEXT_BYTE();
        }

        //TODO : Check packet integrity
        //if (!packet.is_valid())
        //    return false;

        //Save the new start position
        m_start = ROUND(dt_idx);

        //TODO Write exception safe overload of swap
        std::swap(event.m_name, name);
        std::swap(event.m_packet, packet);
        return true;
    }
    catch(std::bad_alloc&)
    {}
    return false;
}

} // namespace SedNL
