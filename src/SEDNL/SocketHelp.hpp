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
#include "SEDNL/Packet.hpp"

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>

typedef uint64_t n_64;
typedef uint32_t n_32;
typedef uint16_t n_16;

#endif /* SEDNL_WINDOWS */

#include <utility>

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
                        struct addrinfo& hints,
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
        std::swap(resources_keeper, inloop_keeper);
    }
}

inline
void __push_16(ByteArray& data, UInt16 dt)
{
    const n_16 ndt = htons(static_cast<n_16>(dt));
    const Byte* const bytes = reinterpret_cast<const Byte*>(&ndt);
    data.push_back(bytes[0]);
    data.push_back(bytes[1]);
}

inline
void __push_32(ByteArray& data, UInt32 dt)
{
    const n_32 ndt = htonl(static_cast<n_32>(dt));
    const Byte* const bytes = reinterpret_cast<const Byte*>(&ndt);
    data.push_back(bytes[0]);
    data.push_back(bytes[1]);
    data.push_back(bytes[2]);
    data.push_back(bytes[3]);
}

static inline
bool __is_big_endian()
{
    const n_64 v = 1;

    return (*reinterpret_cast<const char*>(&v) == 0);
}

static inline
void __bytes_swap(UInt64& v)
{
    n_32 *a = reinterpret_cast<n_32*>(&v);
    n_32 *b = a+1;
    std::swap(*a, *b);
}

static inline
void __push_64(ByteArray& data, UInt64 dt)
{
    if (!__is_big_endian())
        __bytes_swap(dt);
    __push_32(data, static_cast<UInt32>(dt >> 32));
    __push_32(data, static_cast<UInt32>(dt));
}

} // namespace SedNL

#endif /* !SOCKET_HELP_HPP_ */
