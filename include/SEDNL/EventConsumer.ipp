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

#ifndef EVENT_CONSUMER_IPP_
#define EVENT_CONSUMER_IPP_

namespace SedNL
{

Slot<Connection&>& EventConsumer::on_connect()
{
    return m_on_connect_slot;
}

Slot<Connection&>& EventConsumer::on_disconnect()
{
    return m_on_disconnect_slot;
}

Slot<TCPServer&>& EventConsumer::on_server_disconnect()
{
    return m_on_server_disconnect_slot;
}

Slot<Connection&, const Event&>& EventConsumer::on_event()
{
    return m_on_event_slot;
}

Slot<Connection&, const Event&>& EventConsumer::bind(std::string event_name)
{
    return m_slots[event_name];
}

} // namespace SedNL

#endif /* !EVENT_CONSUMER_IPP_ */
