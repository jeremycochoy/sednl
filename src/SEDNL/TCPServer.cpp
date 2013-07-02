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

#include "SEDNL/TCPServer.hpp"
#include "SEDNL/SocketHelp.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/SocketAddress.hpp"

#include <cstring>

namespace SedNL
{

TCPServer::TCPServer() noexcept
{}

TCPServer::TCPServer(const SocketAddress& socket_address)
{
    connect(socket_address);
}

void TCPServer::connect(const SocketAddress& socket_address)
{
    if (!socket_address.is_client_valid())
        throw NetworkException(NetworkExceptionT::InvalidSocketAddress);

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Both IPV6 and IPV4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // We want to bind on

    struct addrinfo* addrs = nullptr;
    bool should_try_again = true;

    //TODO
}

} //namespace SedNL
