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

#include "SEDNL/TCPClient.hpp"
#include "SEDNL/SocketAddress.hpp"
#include "SEDNL/SocketHelp.hpp"
#include "SEDNL/Types.hpp"

#include <cstring>
#include <memory>

namespace SedNL
{

TCPClient::TCPClient() noexcept
{
}

    TCPClient::TCPClient(const SocketAddress& socket_address, int timeout)
    :Connection()
{
    connect(socket_address, timeout);
}

void TCPClient::connect(const SocketAddress& socket_address, int timeout)
{
    if (!socket_address.is_client_valid())
        throw NetworkException(NetworkExceptionT::InvalidSocketAddress);

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Both IPV6 and IPV4
    hints.ai_socktype = SOCK_STREAM; // TCP

    struct addrinfo* addrs = nullptr;

    //Create a lambda deleter
    auto deleter = [](struct addrinfo* ptr)
        {if (ptr != nullptr) {freeaddrinfo(ptr);} };

    //Will store addresses to allow RAII.
    std::unique_ptr<struct addrinfo, decltype(deleter)>
        resources_keeper(nullptr, deleter);

    retrieve_addresses(socket_address.m_name, socket_address.m_port,
                       hints, addrs,
                       resources_keeper, deleter);

    //Socket FileDescriptor
    FileDescriptor fd;
    struct addrinfo *addr = nullptr;
    //Try to connect to each addrinfo
    for (addr = addrs; addr != nullptr; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        //Failure
        if (fd == -1)
            continue;

        //Blocking connect
        if (timeout == 0 || timeout == -1)
        {
            //Success
            if (blocking_connect(fd, addr))
                break;
        }
        //Non blocking connect with timeout
        else
        {
            if (non_blocking_connect(fd, addr, timeout))
                break;
        }

        close(fd);
        //Try again
    }

    //If we failed
    if (addr == nullptr)
    {
        if (timeout != 0 && timeout != -1)
            throw NetworkException(NetworkExceptionT::TimedOut);
        throw NetworkException(NetworkExceptionT::ConnectFailed);
    }

    m_connected = true;
    m_fd = fd;
}

bool TCPClient::blocking_connect(FileDescriptor fd, struct addrinfo *addr)
{
    return (::connect(fd, addr->ai_addr, addr->ai_addrlen) == 0);
}

bool TCPClient::non_blocking_connect(FileDescriptor fd, struct addrinfo *addr,
                                     int timeout)
{
    //Set non-blocking
    if (!set_non_blocking(fd))
        return false;

    //Try to connect
    int errcode = ::connect(fd, addr->ai_addr, addr->ai_addrlen);

    //We are lucky, it worked instantaneously
    if (errcode == 0)
        return true;

    //Failed to start processing connect
    if (EINPROGRESS != errno)
        return false;

    //We have to wait a bit with select
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    errcode = select(fd + 1, nullptr, &wfds, nullptr, &tv);

    //0 == timed out, -1 == error.
    if (errcode == 0 || errcode == -1)
        return false;

    //We should check if connect completed
    int so_error = -1;
    int optlen = sizeof(so_error);

    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, (socklen_t*)&optlen) == -1)
        return false;
    return false;

    //The connection is valid if so_error == 0
    return so_error == 0;
}

} // namespace SedNL
