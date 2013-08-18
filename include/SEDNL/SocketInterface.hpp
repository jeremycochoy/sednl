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

#ifndef SOCKET_INTERFACE_HPP_
#define SOCKET_INTERFACE_HPP_

#include "Export.hpp"
#include "SEDNL/NonCopyable.hpp"
#include "SEDNL/Types.hpp"

#include <mutex>

namespace SedNL
{

///////////////////////////////////////////////////////////////
//! \brief Describe functions that Connection, TCPClient and
//!        TCPServer should have (like a connected
//!        / unconnected state).
///////////////////////////////////////////////////////////////
class SEDNL_API SocketInterface : NonCopyable
{
public:
    //! \brief Initialise member data.
    inline SocketInterface() noexcept;

    //! \brief Should be implemented by client/server/connection.
    virtual void disconnect() noexcept = 0;

    //! \brief Tell if the client/server/connection is connected.
    //! \return True if connected, false otherwise.
    inline bool is_connected() const noexcept;

    //! \brief Call is_connected().
    inline operator bool() const noexcept;

protected:
    //! \brief Connection state.
    bool m_connected;

    //! \brief Socket file descriptor.
    FileDescriptor m_fd;

    //! \brief Mutex used for synchronisation while sending events,
    //!        closing the connection, or changing the user data.
    std::mutex m_mutex;

    inline FileDescriptor get_fd() noexcept;
};

} // namespace SedNL

#include "SEDNL/SocketInterface.ipp"

#endif /* !SOCKET_INTERFACE_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::SocketInterface
//!
//! For use cases, see TCPClient, TCPServer and Connecton
//!  objects.
//!
////////////////////////////////////////////////////////////
