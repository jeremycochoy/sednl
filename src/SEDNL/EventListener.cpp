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
#include "SEDNL/TCPClient.hpp"
#include "SEDNL/SocketHelp.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <cassert>

namespace SedNL
{

EventListener::EventListener(unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
}

EventListener::EventListener(Connection& connection, unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
    attach(connection);
}

EventListener::EventListener(TCPServer& server, unsigned int max_queue_size)
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
void EventListener::__attach(L& list, T* elm)
{
    if (m_running)
        throw EventException(EventExceptionT::EventListenerRunning);
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
void EventListener::__detach(L& list, T* elm)
{
    if (m_running)
        throw EventException(EventExceptionT::EventListenerRunning);
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
bool __epoll_add_fd(int epoll, int fd)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    int err = epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event);
    if (err < 0)
    {
        close(fd);
        return false;
    }
    return true;
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
        if(server->is_connected() && !__epoll_add_fd(epoll, server->m_fd))
            throw EventException(EventExceptionT::EpollCtlFailed);

    //Register clients
    for (auto connection : m_connections)
        if(connection->is_connected() && !__epoll_add_fd(epoll, connection->m_fd))
            throw EventException(EventExceptionT::EpollCtlFailed);

    //Keep the event pool
    std::swap(m_epoll_events, events);
    m_epoll = epoll;
}

bool EventListener::is_server(FileDescriptor fd) const noexcept
{
    for (auto s : m_servers)
    {
        if (s->get_fd() == fd)
            return true;
    }
    return false;
}

//Assume fd is  server
TCPServer *EventListener::get_server(FileDescriptor fd) noexcept
{
    for (auto s : m_servers)
    {
        if (s->get_fd() == fd)
            return s;
    }
    return nullptr;
}

//Assume fd is a server
void EventListener::close_server(FileDescriptor fd)
{
    std::cout << "Asked to close server " << fd << std::endl;//DEBUG
    for (auto s : m_servers)
    {
        if (s->get_fd() == fd)
        {
            //Create event
            tell_disconnected(s);
            //Close the server
            close(s->get_fd());
            return;
        }
    }
}

template <class T, class U>
static inline
void disconnected_event(T &queue, U cn, FileDescriptor fd, const char* type)
{
    if (!queue.push(cn))
    {
#ifndef SEDNL_NOWARN
        std::cerr << "Error: "
                  << "Lost a " << type << " disconnected event for server "
                  << fd
                  << std::endl;
#endif /* SEDNL_NOWARN */
    }
}

//Assume fd is a connection
void EventListener::close_connection(FileDescriptor fd)
{
    std::shared_ptr<Connection> cn;

    //Look on the "external list"
    if (!m_connections.empty()) //rely on branch prediction on empty list
    {
        for (auto connection : m_connections)
            //We create a 'false' shared_ptr (i.e. without destructor)
            if (connection->get_fd() == fd)
            {
                cn = decltype(cn)(connection, [](Connection*){});
                break;
            }
    }

    //Look on the "internal list"
    if (!cn) //branch prediction ...
    {
        auto it = m_internal_connections.find(fd);
        if (it != m_internal_connections.end())
        {
            cn = it->second;
            m_internal_connections.erase(it);
        }
    }

    std::cout << "Asked to close " << fd << std::endl;//DEBUG
    cn->safe_disconnect();
    //Create the server disconnected event
    disconnected_event(m_disconnected_queue, cn, cn->get_fd(), "connection");
}

//Assume fd is a server
void EventListener::accept_connections(FileDescriptor fd)
{
    TCPServer* server = get_server(fd);
    std::cout << "accept" << std::endl;//DEBUG
    while (true)
    {
        struct sockaddr in_addr;
        socklen_t in_len;
        FileDescriptor cfd = accept(fd, &in_addr, &in_len);
        if (cfd < 0)
        {
            //No more fd
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't accept on server socket "
                      << fd
                      << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* SEDNL_NOWARN */
            return;
        }

        if (set_non_blocking(cfd) < 0)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't set accepted socket non-blocking "
                      << cfd
                      << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* SEDNL_NOWARN */
            close(cfd);
            return;
        }

        if (__epoll_add_fd(m_epoll, cfd) < 0)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't accepted socket to epoll "
                      << cfd
                      << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* SEDNL_NOWARN */
            close(cfd);
            return;
        }

        //Create the connection
        InternalList::iterator it;
        try
        {
            //Add connection
            auto cn = std::shared_ptr<Connection>(new Connection);
            cn->m_listener = this;
            cn->m_connected = true;
            cn->m_fd = cfd;
            auto p = m_internal_connections.emplace(cfd, cn);
            //We assert the insertion can't fail
            //There shouldn't be an old cfd file descriptor
            assert(p.second == true);
            it = p.first;
        }
        //Catch std::bad_alloc and others
        catch(std::exception &e)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't create/store connection "
                      << cfd
                      << std::endl;
            std::cerr << "    " << e.what() << std::endl;
#endif /* SEDNL_NOWARN */
            close(cfd);
            return;
        }

        //Create the event
        if (!m_connected_queue.push(it->second))
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't create 'connected' event "
                      << cfd
                      << std::endl;
#endif /* SEDNL_NOWARN */
            m_internal_connections.erase(it);
            close(cfd);
            return;
        }

        //We did it!

        //DEBUG
        std::cout << "Connection " << cfd << " accepted!" << std::endl;
    }
}


//Assume fd is a connection
void EventListener::read_connection(FileDescriptor fd)
{
    auto cn = get_connection(fd);
    std::cout << "read" << std::endl; //DEBUG
    //TODO
}

std::shared_ptr<Connection>
EventListener::get_connection(FileDescriptor fd) noexcept
{
    if (!m_connections.empty())
    {
        for (auto connection : m_connections)
            if (connection->get_fd() == fd)
                return std::shared_ptr<Connection>
                    (connection, [](Connection*){});
    }

    auto it = m_internal_connections.find(fd);
    if (it != m_internal_connections.end())
        return it->second;

    return std::shared_ptr<Connection>();
}

//We are called with the m_fd lock
void EventListener::tell_disconnected(Connection *cn) noexcept
{
    std::shared_ptr<Connection> ptr;

    if (!m_connections.empty())
    {
        for (auto connection : m_connections)
            if (connection == cn)
            {
                ptr = decltype(ptr)(cn, [](Connection*){});
                break;
            }
    }
    else
    {
        for (auto it = m_internal_connections.begin();
             it != m_internal_connections.end();
             it++)
            if (it->second.get() == cn)
            {
                ptr = it->second;
                //Remove cn from internal connections
                m_internal_connections.erase(it);
                break;
            }
    }

    if (!ptr)
        return;

    disconnected_event(m_disconnected_queue, ptr, ptr->m_fd, "connection");

    std::cout << "Tell disconnect for connection" << ptr->m_fd << std::endl; //DEBUG
}

//We are called with the m_fd lock
void EventListener::tell_disconnected(TCPServer *s) noexcept
{
    disconnected_event(m_server_disconnected_queue, s, s->get_fd(), "server");
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
        int n = epoll_wait(m_epoll, m_epoll_events.get(), MAX_EVENTS, 100);

        for (int i = 0; i < n; i++)
        {
            FileDescriptor event_fd = m_epoll_events[i].data.fd;
            //An error occured or the connection was closed
            if (m_epoll_events[i].events & EPOLLERR
                || m_epoll_events[i].events & EPOLLHUP)
            {
                if (is_server(event_fd))
                    close_server(event_fd);
                else
                    close_connection(event_fd);
                continue;
            }

            //SHOULDN't HAPPEN! If it happens, we ignore it
            if (!(m_epoll_events[i].events & EPOLLIN))
            {
#ifndef SEDNL_NOWARN
                std::cerr << "Warning: "
                          << "epoll_wait() returned a non EPOLLIN event."
                          << std::endl;
#endif /* !SEDNL_NOWARN */
                continue;
            }

            //So, it's a read avent.

            //Incoming connection
            if (is_server(event_fd))
            {
                accept_connections(event_fd);
                continue;
            }
            //It's a read on a connection
            else
            {
                read_connection(event_fd);
                continue;
            }
        }
    }

    //Close all connections (and create events)
    for (auto cn : m_internal_connections)
        cn.second->disconnect();

    //TODO : Join consumer threads

    //Release resources
    close(m_epoll);
    m_epoll = -1;
    m_epoll_events.release();
    m_internal_connections.clear();

    std::cout << "EventListener released" << std::endl;//DEBUG
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
