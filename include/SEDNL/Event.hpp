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

class Packet;
class RingBuf;

class SEDNL_API Event
{
public:
    //! \brief Construct an empty event
    //!
    //! \param[in] name Name of the event
    inline Event(std::string name = "");

    //! \brief Construct an event from a packet
    //!
    //! \param[in] name Name of the event
    //! \param[in] packet The data attached
    inline Event(std::string name, const Packet& packet);

    //! \brief Return a reference to the packet
    //!
    //! \return The currently handled packet
    inline const Packet& get_packet() const noexcept;

    //! \brief Return the event name
    //!
    //! Notice it's actualy a const reference to
    //! the handler. Make a copy if you expect
    //! the event to die before the string.
    //!
    //! \return A handler to the event name
    inline const std::string& get_name() const noexcept;

    //! \brief Compute the binary header of an event
    //!
    //! Low level function.
    //!
    //! \return The binary header.
    ByteArray get_header() const;

    //! \brief Compute a binary event ready to send
    //!
    //! Low level function. Return the concatenated value
    //! of Event::get_header() and Packet::get_data().
    //!
    //! \return The serialised event as a binary string
    ByteArray pack() const;

private:
    std::string m_name;
    Packet m_packet;

    //! Allowed to create an event from data
    friend RingBuf;
};

} // namespace SedNL

#include "SEDNL/Event.ipp"

#endif /* !EVENT_HPP_ */
