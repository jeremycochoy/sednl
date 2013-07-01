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

#include <cstring>
#include <memory>

namespace SedNL
{

TCPClient::TCPClient() noexcept
{
}

TCPClient::TCPClient(const SocketAddress& socket_address)
    :Connection()
{
    connect(socket_address);
}

void TCPClient::connect(const SocketAddress& socket_address)
{
    if (!socket_address.is_client_valid())
        throw NetworkException(NetworkExceptionT::InvalidSocketAddress);

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Both IPV6 and IPV4
    hints.ai_socktype = SOCK_STREAM; // TCP

    struct addrinfo* addrs = nullptr;
    bool should_try_again = true;

    //Create a lambda deleter
    auto deleter = [](struct addrinfo* ptr)
        {if (ptr != nullptr) {freeaddrinfo(ptr);} };

    //Will store addresses to allow RAII.
    std::unique_ptr<struct addrinfo, decltype(deleter)>
        resources_keeper(nullptr, deleter);

    //Allow two tries
    for (int i = 0; i < 2; i++)
    {
        int errcode = getaddrinfo(socket_address.m_name.c_str(),
                                  std::to_string(socket_address.m_port).c_str(),
                                  &hints, &addrs);

        std::unique_ptr<struct addrinfo, decltype(deleter)>
            inloop_keeper(addrs, deleter);

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

        //Success
        if (::connect(fd, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        close(fd);
        //Try again
    }

    //If we failed
    if (addr == nullptr)
        throw NetworkException(NetworkExceptionT::ConnectFailed);

    m_connected = true;
    m_fd = fd;
}

void TCPClient::disconnect() noexcept
{
    //The call to close may fail, for example if the last write failed.
    //But what can we do to that? So we silently ignore the return value.
    close(m_fd);
}

}
