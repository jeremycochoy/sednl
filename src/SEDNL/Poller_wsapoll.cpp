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
#ifdef SEDNL_BACKEND_WSAPOLL

// WARNING : Highly experimental!

#include "SEDNL/Types.hpp"
#include "SEDNL/Poller.hpp"

#include <cstring>

namespace SedNL
{

Poller::Poller()
    :m_nb_events(0), m_idx(0)
{
    memset(m_events, 0, sizeof(*m_events) * MAX_EVENTS);
    for (int i = 0; i < MAX_EVENTS; i++)
        m_events[i].fd = INVALID_SOCKET;
}

Poller::~Poller()
{
}

//Close fd befor throwing, to prevent resource licking
bool Poller::add_fd(FileDescriptor fd) noexcept
{
    int i = 0;
    while (i < MAX_EVENTS && m_events[i].fd != INVALID_SOCKET)
        i++;

    if (i == MAX_EVENTS)
        return false;

    m_events[i].fd = fd;
    m_events[i].events = POLLIN;

    return true;
}

void Poller::remove_fd(FileDescriptor fd) noexcept
{
    for (int i = 0; i < MAX_EVENTS; i++)
        if (m_events[i].fd == reinterpret_cast<unsigned int&>(fd))
        {
            m_events[i].fd = -1;
            m_events[i].events = 0;
            m_events[i].revents = 0;
            return;
        }
}

void Poller::wait_for_events(int timeout) noexcept
{
    m_nb_events = WSAPoll(m_events, MAX_EVENTS, timeout);
    m_idx = 0;
    m_cur_ev = 0;
}

bool Poller::next_event(Event& e) noexcept
{
    if (m_cur_ev >= m_nb_events)
        return false;

    while (m_idx < MAX_EVENTS &&
           (m_events[m_idx].fd == INVALID_SOCKET
            || m_events[m_idx].revents == 0))
        m_idx++;

    if (m_idx >= MAX_EVENTS)
        return false;

    e.fd = m_events[m_idx].fd;
    //An error occured or the connection was closed
    e.is_close = m_events[m_idx].revents & POLLHUP
              || m_events[m_idx].revents & POLLNVAL;
    //Ready to read
    e.is_read = m_events[m_idx].revents & POLLIN;

    m_idx++;
    m_cur_ev++;

    return true;
}

} // namespace SedNL

#endif /* SEDNL_BACKEND_WSAPOLL */
