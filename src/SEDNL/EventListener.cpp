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

#include "SEDNL/EventListener.hpp"
#include "SEDNL/Connection.hpp"
#include "SEDNL/TCPServer.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace SedNL
{

EventListener::EventListener(unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
}

EventListener::EventListener(Connection &connection, unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
    attach(connection);
}

EventListener::EventListener(TCPServer &server, unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
    attach(server);
}

//
// When an object is attached/detached, a pointer to the listener
// is stored inside this object. We also have to check that we
// detach from the right listener.
//

template<class L, class T> inline
void EventListener::__attach(L& list, T *elm)
{
    auto res = std::find(list.begin(), list.end(), elm);
    if (res == list.end())
    {
        if (elm->m_listener != nullptr && elm->m_listener != this)
            throw EventException(EventExceptionT::AlreadyListened);
        elm->m_listener = this;
        list.push_back(elm);
    }
}

template<class L, class T> inline
void EventListener::__detach(L& list, T *elm)
{
    auto res = std::find(list.begin(), list.end(), elm);
    if (res != list.end())
    {
        if (elm->m_listener != this)
            throw EventException(EventExceptionT::WrongParentListener);
        elm->m_listener = nullptr;
        list.erase(res);
    }
}

void EventListener::attach(TCPServer &server) throw(std::bad_alloc, EventException)
{
    __attach(m_servers, &server);
}

void EventListener::attach(Connection &connection) throw(std::bad_alloc, EventException)
{
    __attach(m_connections, &connection);
}

void EventListener::detach(TCPServer &server) throw(std::bad_alloc, EventException)
{
    __detach(m_servers, &server);
}

void EventListener::detach(Connection &connection) throw(std::bad_alloc, EventException)
{
    __detach(m_connections, &connection);
}

//Close fd befor throwing, to prevent resource licking
static inline
void __epoll_add_fd(int epoll, int fd)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    int err = epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event);
    if (err < 0)
    {
        close(fd);
        throw EventException(EventExceptionT::EpollCtlFailed);
    }
}

void EventListener::run_init()
{
    //////////////////////////
    // LINUX IMPLEMENTATION //
    //////////////////////////

    //Here, we can throw a std::bad_alloc
    auto events = std::unique_ptr<struct epoll_event[]>
        (new struct epoll_event[MAX_EVENTS]);

    //Create epoll
    FileDescriptor epoll = epoll_create(EPOLL_SIZE);
    if (epoll < 0)
        throw EventException(EventExceptionT::EpollCreateFailed);

    //If __epoll_add_fd fail, it close the epoll file descriptor and
    // throw an exception.

    //Register servers
    for (auto server : m_servers)
        __epoll_add_fd(epoll, server->m_fd);

    //Register clients
    for (auto connection : m_connections)
        __epoll_add_fd(epoll, connection->m_fd);

    //Keep the event pool
    std::swap(m_events, events);
    m_epoll = epoll;
}

bool EventListener::is_server(FileDescriptor fd)
{
    //TODO
    return false;
}

TCPServer *EventListener::get_server(FileDescriptor fd)
{
    //TODO
    return nullptr;
}

void EventListener::close_server(FileDescriptor fd)
{
    //TODO
    // Remember we should create a server_closed event
}

void EventListener::close_connection(FileDescriptor fd)
{
    //TODO
}

void EventListener::run_imp()
{
    //////////////////////////
    // LINUX IMPLEMENTATION //
    //////////////////////////

    //Since we can't assume reading m_running is atomic, we use a mutex
    while (m_running) //event loop
    {
        //Wait ~100ms, and check events (this allow to EventListener::join
        // even if nothing happens)
        int n = epoll_wait(m_epoll, m_events.get(), MAX_EVENTS, 100);
        for (int i = 0; i < n; i++)
        {
            //An error occured or the connection was closed
            if (m_events[i].events & EPOLLERR || m_events[i].events & EPOLLHUP)
            {
                if (is_server(m_events[i].data.fd))
                    close_server(m_events[i].data.fd);
                else
                    close_connection(m_events[i].data.fd);
                continue;
            }

            //SHOULDN't HAPPEN! If it happens, we ignore it
            if (!(m_events[i].events & EPOLLIN))
            {
#ifndef SEDNL_NOWARN
                std::cerr << "Warning: "
                          << "epoll_wait() returned a non EPOLLIN event."
                          << std::endl;
#endif /* !SEDNL_NOWARN */
                continue;
            }
        }
    }

    //Release resources
    close(m_epoll);
    m_epoll = -1;
    m_events.release();
}

void EventListener::run() throw(EventException)
{
    //Create the thread if it's not already running
    if (!m_running)
    {
        run_init();

        //Once everything is right, we launch the thread
        m_running = true;
        m_thread = std::thread(std::bind(&EventListener::run_imp, this));
    }
    else
        throw EventException(EventExceptionT::EventListenerRunning);
}

void EventListener::join()
{
    if (m_running)
    {
        m_running = false;
        if (m_thread.joinable() == true)
            m_thread.join();
    }
}

} // namespace SedNL
