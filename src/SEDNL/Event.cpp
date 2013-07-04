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

#include "SEDNL/Event.hpp"
#include "SEDNL/Packet.hpp"
#include "SocketHelp.hpp"

namespace SedNL
{

ByteArray Event::get_header() const
{
    const auto data = m_packet.get_data();
    //Size of the packet = |length : UInt16| + |m_name . '\0'| + |Packet|
    const UInt16 length = data.size() + m_name.length() + 1 + sizeof(UInt16);

    ByteArray header;
    __push_16(header, length);
    header.insert(header.end(), m_name.begin(), m_name.end());
    header.push_back('\0');

    return header;
}

ByteArray Event::pack() const
{
    ByteArray ev = get_header();

    const ByteArray data = m_packet.get_data();
    ev.insert(ev.end(), data.begin(), data.end());

    return ev;
}

} // namespace SedNL
