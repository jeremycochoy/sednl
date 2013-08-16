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

#include "SEDNL/SocketAddress.hpp"

namespace SedNL
{

SocketAddress::SocketAddress()
    :m_port(0), m_name(""), m_empty(true)
{}

SocketAddress::SocketAddress(int port)
    :m_port(port), m_name(""), m_empty(false)
{}

SocketAddress::SocketAddress(int port, std::string server_name)
    :m_port(port), m_name(server_name), m_empty(false)
{}

bool SocketAddress::is_client_valid() const noexcept
{
    return (m_name != "" && !m_empty);
}

bool SocketAddress::is_server_valid() const noexcept
{
    return (!m_empty);
}

} // namespace SedNL

#ifdef SEDNL_WINDOWS

#include "windows.h"

namespace SedNL
{

/* Windows need initialisation/cleanup to use sockets. */
struct SocketInit
{
    SocketInit()
    {
        WSADATA v;
        WSAStartup(MAKEWORD(2, 2), &v);
    }
    ~SocketInit()
    {
        WSACleanup();
    }
};

SocketInit windows_socket_init;

}

#endif /* SEDNL_WINDOWS */

