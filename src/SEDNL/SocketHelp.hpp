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

#ifndef SOCKET_HELP_HPP_
#define SOCKET_HELP_HPP_

#include "SEDNL/Exception.hpp"

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <unistd.h>
#include <fcntl.h>

#endif /* SEDNL_WINDOWS */

namespace SedNL
{

//! \brief Set a socket file descriptor non blocking
inline bool set_non_blocking(int fd)
{
    int flags = 0;
    if ((flags = fcntl(fd, F_GETFL)) < 0)
        return false;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0)
        return false;
    return true;
}

//! Try to obtain addresses with getaddrinfo
//! See TCPServer::connect() and TCPClient::connect().
template<typename T, typename U>
void retrieve_addresses(std::string sa_node, int sa_port,
                        struct addrinfo &hints,
                        struct addrinfo*& addrs,
                        T& resources_keeper, U deleter)
{
    bool should_try_again = true;

    // Port
    std::string port = std::to_string(sa_port);

    //Allow two tries
    for (int i = 0; i < 2; i++)
    {
        int errcode;
        if(sa_node != "")
            errcode = getaddrinfo(sa_node.c_str(),
                                  port.c_str(),
                                  &hints, &addrs);
        else
            errcode = getaddrinfo(nullptr,
                                  port.c_str(),
                                  &hints, &addrs);

        T inloop_keeper(addrs, deleter);

        //Check the error code. Depending of the error, we may allow one more try.
        if (errcode < 0)
        {
            switch (errcode)
            {
            case EAI_MEMORY:
                throw std::bad_alloc();
            case EAI_AGAIN:
                if (should_try_again)
                {
                    //TODO: Add a small 1ms sleep to let the kernel do some work and
                    //      try again.
                    break;
                }
            default:
                throw NetworkException(NetworkExceptionT::CantRetrieveHost,
                                       gai_strerror(errcode));
            }
        }
        resources_keeper.swap(inloop_keeper);
    }
}

} // namespace SedNL

#endif /* !SOCKET_HELP_HPP_ */
