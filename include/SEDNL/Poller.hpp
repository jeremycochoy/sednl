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

#ifndef POLLER_HPP_
#define POLLER_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/SocketHelp.hpp"

#ifndef EPOLL_SIZE
# define EPOLL_SIZE MAX_CONNECTIONS
#endif /* !EPOLL_SIZE */
#ifndef MAX_EVENTS
# define MAX_EVENTS 256
#endif /* !MAX_EVENTS */

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <sys/epoll.h>

#endif /* SEDNL_WINDOWS */

#include <memory>

namespace SedNL
{

class Poller
{
public:
    SEDNL_API Poller();
    SEDNL_API ~Poller();

    struct Event
    {
        FileDescriptor fd;
        bool is_close;
        bool is_read;
    };

    //! \brief In case of fails, it close the poller
    bool add_fd(FileDescriptor fd) noexcept;

    //! \brief Remove a file descriptor from the poll
    void remove_fd(FileDescriptor fd) noexcept;

    //! \brief Return the number of events
    int wait_for_events(int timeout) noexcept;

    //! \brief return true and modify e if they are one more event
    bool next_event(Event& e) noexcept;

private:
    FileDescriptor m_epoll;
    std::unique_ptr<struct epoll_event[]> m_events;
    unsigned int m_nb_events;
    unsigned int m_idx;
};

} // namespace SedNL

#endif /* !POLLER_HPP_ */

