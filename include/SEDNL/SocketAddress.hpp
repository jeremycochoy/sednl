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

#ifndef SOCKET_ADDRESS_HPP_
#define SOCKET_ADDRESS_HPP_

#include "SEDNL/Export.hpp"

#include <string>

namespace SedNL
{

class SEDNL_API SocketAddress
{
public:
    //! \brief Construct an empty SocketAddress
    SocketAddress();

    //! \brief Construct a SocketAddress containing only a port number
    SocketAddress(int port);

    //! \brief Construct a SocketAddress containing port and server name
    SocketAddress(int port, std::string server_name);

    //! \brief Tell if it's a valid TCPClient SocketAddress
    bool is_client_valid() const noexcept;

    //! \brief Tell if it's a valid TCPServer SocketAddress
    bool is_server_valid() const noexcept;

private:
    //! \brief Port to connect to, or to listen
    int m_port;

    //! \brief Server name, for a TCPClient
    std::string m_name;

    //! \brief Tell if it's an empty socket address.
    bool m_empty;

    friend class TCPClient;
    friend class TCPServer;
};

} // namespace SedNL

#endif /* !SOCKET_ADDRESS_HPP_ */
