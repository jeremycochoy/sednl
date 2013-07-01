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

namespace SedNL
{

    class SocketAddress;

////////////////////////////////////////////////////////////
//! \brief Create a connection to a server
////////////////////////////////////////////////////////////
class SEDNL_API TCPClient : Connection
{
public:
    //! \brief Create an empty client
    TCPClient();

    //! \brief Create a connection to the SocketAddress
    //!
    //! Calling this constructor is the same as creating an
    //! empty TCPClient and then calling connect().
    //!
    //! \argument socket_address Server and port to connect to
    TCPClient(const SocketAddress& socket_address);

    //! \brief Connect to a SocketAddress
    //!
    //! If the connection was already openend, it thrown
    //! an exception. You can check if the connection was already
    //! openend with is_open().
    //!
    //! \argument socket_address Server and port to connect to
    void connect(const SocketAddress& socket_address);
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
//! TCPConnection connection(SocketAddress(4242, "localhost"));
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
