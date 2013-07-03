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

#ifndef TCP_CLIENT_HPP_
#define TCP_CLIENT_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/Connection.hpp"

#include <vector>

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

    //! A binary array
    typedef std::vector<Byte> ByteArray;

////////////////////////////////////////////////////////////
//! \brief Create a connection to a server
////////////////////////////////////////////////////////////
class SEDNL_API TCPClient : public Connection
{
public:
    //! \brief Create an empty client
    TCPClient() noexcept;

    //! \brief Create a connection to the SocketAddress
    //!
    //! Calling this constructor is the same as creating an
    //! empty TCPClient and then calling TCPClient::connect().
    //!
    //! \argument[in] socket_address Server and port to connect to
    //! \argument[in] timeout Socket timeout in miliseconds.
    //!                       See TCPClient::connect.
    TCPClient(const SocketAddress& socket_address, int timeout = 1000);

    //! \brief Connect to a SocketAddress
    //!
    //! If the connection was already openend, it thrown
    //! an exception. You can check if the connection was already
    //! openend with is_open().
    //!
    //! You can give the timeout you allow to connect.
    //! If you want to block until sucess or failure
    //! (it's discouraged), put 0 or -1. You'll then
    //! have a "blocking" connect call.
    //! You should know that it doesn't means that
    //! connect will return after timeout miliseconds.
    //! Actualy, connect will try with each available
    //! server ip / connection interface, and for each
    //! of them, connect will wait timeout ms.
    //! For example, if google is unreachable, since
    //! google.fr have 4 ips, it will try at least 4
    //! times.
    //!
    //! \argument[in] socket_address Server and port to connect to
    //! \argument[in] timeout The socket timeout in miliseconds.
    void connect(const SocketAddress& socket_address, int timeout = 1000);

private:

    //! \brief Try to connect with a call to 'connect'.
    //!
    //! If the function success, fd is now a blocking socket.
    //!
    //! \argument[in] fd The socket file descriptor
    //! \argument[in] addr The addrinfo description of the server
    bool blocking_connect(FileDescriptor fd, struct addrinfo *addr);

    //! \brief Try to connect with connect, select, and non blocking socket.
    //!
    //! If the function success, fd is now a nonblocking socket.
    //!
    //! \argument[in] fd The socket file descriptor
    //! \argument[in] addr The addrinfo description of the server
    //! \argument[in] timeout Select's timeout in ms
    bool non_blocking_connect(FileDescriptor fd, struct addrinfo *addr, int timeout);
};

} // namespace SedNL

#endif /* !TCPConnection */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::TCPClient
//!
//! Here is a basic use of a TCPClient :
//!
//! \code
//!
//! //Create a connection
//! TCPClient connection(SocketAddress(4242, "localhost"));
//!
//! //Create an empty event
//! Event event("hello_msg");
//!
//! //Send it
//! connection.send(event);
//!
//! //Close the connection
//! connection.disconnect();
//!
//! \endcode
////////////////////////////////////////////////////////////
