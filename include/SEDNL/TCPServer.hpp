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

#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/SocketInterface.hpp"

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#endif /* SEDNL_WINDOWS */

namespace SedNL
{

class SocketAddress;
class EventListener;

class SEDNL_API TCPServer : public SocketInterface
{
public:
    //! \brief Create an empty server
    TCPServer() noexcept;

    //! \brief Close the connection if needed
    ~TCPServer() noexcept;

    //! \brief Create a server connection on the SocketAddress
    //!
    //! Calling this constructor is the same as creating an
    //! empty TCPServer, then calling TCPServer::connect().
    //!
    //! \param[in] socket_address Describe the port on wich the server
    //!                           will listen.
    TCPServer(const SocketAddress &socket_address);

    //! \brief Create a server connection on the SocketAddress
    //!
    //! Calling this constructor is the same as creating an
    //! empty TCPServer, then calling TCPServer::connect().
    //!
    //! \param[in] socket_address Describe the port on wich the server
    //!                           will listen.
    void connect(const SocketAddress &socket_address);

    //! \brief Cut the connection
    virtual void disconnect() noexcept;

private:
    //! \brief the current listener
    EventListener *m_listener;

    void unsafe_disconnect() noexcept;
    void safe_disconnect() noexcept;

    friend class EventListener;
};

} // namespace SedNL

#endif /* !TCP_SERVER_HPP_ */
