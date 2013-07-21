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

#ifndef SOCKET_INTERFACE_IPP_
#define SOCKET_INTERFACE_IPP_

#include "Export.hpp"

#include <iostream>

namespace SedNL
{

SocketInterface::SocketInterface() noexcept
    :m_connected(false), m_fd(-1)
{}

bool SocketInterface::is_connected() const noexcept
{
    return m_connected;
}

SocketInterface::operator bool() const noexcept
{
    return is_connected();
}

inline
FileDescriptor SocketInterface::get_fd() noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_fd;
    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
    std::cerr << "Error: "
              << "std::mutex::lock failed in Connection::get_fd" << std::endl;
    std::cerr << "    " << e.what() << std::endl;
#endif /* !SEDNL_NOWARN */
        return m_fd;
    }
}

} // namespace Sednl

#endif /* !SOCKET_INTERFACE_IPP_ */
