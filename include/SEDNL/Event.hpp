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

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/Packet.hpp"

#include <string>

namespace SedNL
{

class SEDNL_API Event
{
public:
    //! \brief Construct an empty event.
    //!
    //! \param[in] name Name of the event.
    inline Event(std::string name = "");

    //! \brief Construct an event from a packet.
    //!
    //! \param[in] name Name of the event.
    //! \param[in] packet The data attached.
    inline Event(std::string name, const Packet& packet);

    //! \brief Return a reference to the packet handled.
    //!
    //! Once this event is destructed, the packet will be
    //! destructed and the reference invalid.
    //!
    //! \return The currently handled packet.
    inline const Packet& get_packet() const noexcept;

    //! \brief Return the event name.
    //!
    //! Notice it's actualy a const reference to
    //! the handler. Make a copy if you expect
    //! the event to die before the string.
    //!
    //! \return A handler to the event name.
    inline const std::string& get_name() const noexcept;

    //! \brief Compute the binary header of an event.
    //!
    //! Low level function, used by the implementation.
    //!
    //! You probably don't want to use it.
    //!
    //! \return The binary header.
    ByteArray get_header() const;

    //! \brief Compute a binary string representing the event.
    //!
    //! Low level function. Return the concatenated value
    //! of Event::get_header() and Packet::get_data().
    //!
    //! \return The serialised event as a binary string.
    ByteArray pack() const;

private:
    //! \brief Event name.
    std::string m_name;

    //! \brief Packet containing data.
    Packet m_packet;

    friend RingBuf;
};

//! \brief Allow creating easily new events.
//!
//! You can create an event with make_event(event_name, arg1, arg2, ...).
//!
//! \return The newly created event.
template<typename... Args>
Event make_packet(const std::string& event_name, Args... args);

//! \brief Display an Event in a JSON like format.
SEDNL_API std::ostream&  operator<< (std::ostream& os, const Event& e);

} // namespace SedNL

#include "SEDNL/Event.ipp"

#endif /* !EVENT_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::Event
//!
//! The Event object is an event as send by TCPConnect::send
//! or received by EventListener and given to your callbacks
//! through EventConsumer.
//!
//! Event are basically a "named Packet", where Packet contain
//! the data you want to send / you just received.
//!
//! \code
//!
//! //Create an event
//! Event event("hit_monster", make_packet(damage));
//!
//! //Send it
//! connection.send(event);
//!
//! //Extract data from an event (make a mutable copy)
//! std::string event_name = event.get_name();
//! Packet packet = event.get_packet();
//!
//! \endcode
////////////////////////////////////////////////////////////
