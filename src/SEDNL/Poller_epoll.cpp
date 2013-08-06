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

#include "SEDNL/Types.hpp"
#include "SEDNL/Poller.hpp"

#include <cstring>

//DEBUG:
#define SEDNL_BACKEND_EPOLL

// This is the linux epoll backend

#ifdef SEDNL_BACKEND_EPOLL

namespace SedNL
{

Poller::Poller()
    :m_epoll(-1), m_events(new struct epoll_event[MAX_EVENTS])
{
    bzero(m_events.get(), sizeof(*m_events.get()) * MAX_EVENTS);
    //Create epoll
    m_epoll = epoll_create(EPOLL_SIZE);
    if (m_epoll < 0)
        throw EventException(EventExceptionT::PollerCreateFailed);
}

Poller::~Poller()
{
    if (m_epoll != -1)
        close(m_epoll);
}

//Close fd befor throwing, to prevent resource licking
bool Poller::add_fd(FileDescriptor fd)
{
    struct epoll_event event;

    if (m_epoll < 0)
        return false;

    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

    const int err = epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &event);
    if (err < 0)
        return false;
    return true;
}

void remove_fd(FileDescriptor /*fd*/)
{
    /*
      EPOLL automatically remove closed FDs.
    */
}

int Poller::wait_for_events(int timeout)
{
    return epoll_wait(m_epoll, m_events.get(), MAX_EVENTS, timeout);
}

//getNextEvent

} // namespace SedNL

#endif /* !SEDNL_BACKEND_EPOLL */
