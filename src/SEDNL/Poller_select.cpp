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

// This is the linux epoll backend

// This flag is activated at compile time
#ifdef SEDNL_BACKEND_SELECT

#include "SEDNL/Types.hpp"
#include "SEDNL/Poller.hpp"

#ifdef SEDNL_WINDOWS

#define FD_SET_SIZE FD_SETSIZE

#else /* SEDNL_WINDOWS */

#include <sys/select.h>
#include <sys/time.h>

#endif /* SEDNL_WINDOWS */

#include <cstring>

namespace SedNL
{

Poller::Poller()
    :m_nfds(-1), m_idx(0)
{
    FD_ZERO(&m_readfds);

    //Useless since we have m_tmp_xx = m_xx.
    //But for safety...
    FD_ZERO(&m_tmp_readfds);
}

Poller::~Poller()
{}

bool Poller::add_fd(FileDescriptor fd) noexcept
{
#ifndef SEDNL_WINDOWS
    if (fd >= FD_SETSIZE || fd < 0)
        return false;
#endif /* !SEDNL_WINDOWS */

    FD_SET(fd, &m_readfds);
    std::cout << "add fd " << fd << std::endl;
#ifndef SEDNL_WINDOWS
    if (m_nfds <= fd)
        m_nfds = fd + 1;
#else /* !SEDNL_WINDOWS */
    m_nfds = FD_SET_SIZE;
#endif /* !SEDNL_WINDOWS */
    return true;
}

void Poller::remove_fd(FileDescriptor fd) noexcept
{
#ifndef SEDNL_WINDOWS
    if (fd >= FD_SETSIZE || fd < 0)
        return;
#endif /* !SEDNL_WINDOWS */

    FD_CLR(reinterpret_cast<unsigned int&>(fd), &m_readfds);
}

void Poller::wait_for_events(int timeout) noexcept
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000;

    m_tmp_readfds = m_readfds;
    select(m_nfds, &m_tmp_readfds, nullptr, nullptr, &tv);

    m_idx = -1;
}

bool Poller::next_event(Event& e) noexcept
{
    if (m_idx >= FD_SETSIZE)
        return false;

#ifndef SEDNL_WINDOWS
    while (++m_idx < m_nfds)
    {
        if (FD_ISSET(m_idx, &m_tmp_readfds))
            break;
    }

    if (m_idx >= m_nfds)
        return false;

    e.fd = m_idx;
    e.is_close = false;
    e.is_read = FD_ISSET(m_idx, &m_tmp_readfds);
#else /* !SEDNL_WINDOWS */
    while (++m_idx < FD_SETSIZE)
    {
      if (FD_ISSET(m_readfds.fd_array[m_idx], &m_tmp_readfds))
            break;
    }

    if (m_idx >= FD_SETSIZE)
        return false;

    e.fd = m_tmp_readfds.fd_array[m_idx];
    e.is_close = false;
    e.is_read = FD_ISSET(m_readfds.fd_array[m_idx], &m_tmp_readfds);
#endif /* !SEDNL_WINDOWS */

    return true;
}

} // namespace SedNL

#endif /* SEDNL_BACKEND_SELECT */
