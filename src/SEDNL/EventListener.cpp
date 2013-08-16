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
#include "SEDNL/EventConsumer.hpp"
#include "SEDNL/Connection.hpp"
#include "SEDNL/TCPServer.hpp"
#include "SEDNL/TCPClient.hpp"
#include "SEDNL/SocketHelp.hpp"
#include "SEDNL/Poller.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <cassert>

namespace SedNL
{

EventListener::EventListener(unsigned int max_queue_size)
    :m_max_queue_size(max_queue_size), m_running(false)
{
    clear_consumer_links();
}

EventListener::EventListener(Connection& connection, unsigned int max_queue_size)
    :EventListener(max_queue_size)
{
    attach(connection);
}

EventListener::EventListener(TCPServer& server, unsigned int max_queue_size)
    :EventListener(max_queue_size)
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

void EventListener::clear_consumer_links() noexcept
{
    m_on_disconnect_link = nullptr;
    m_on_server_disconnect_link = nullptr;
    m_on_event_link = nullptr;
    m_links.clear();
}

template<typename S>
void EventListener::link_consumer(EventConsumer* consumer,
                                  S& slot,
                                  ConsumerDescriptor*& link)
throw(EventException)
{
    if (slot)
    {
        if (link)
        {
            clear_consumer_links();
            throw EventException(EventExceptionT::EventCollision);
        }
        else
            link = &consumer->m_descriptor;
    }
}

void EventListener::run_init()
{
    //////////////////////////
    // LINUX IMPLEMENTATION //
    //////////////////////////

    //If __epoll_add_fd fail, it close the epoll file descriptor and
    // throw an exception.
    std::unique_ptr<Poller> poller(new Poller);

    //Register servers
    for (auto server : m_servers)
        if(server->is_connected() && !poller->add_fd(server->m_fd))
            throw EventException(EventExceptionT::PollerAddFailed);

    //Register clients
    for (auto connection : m_connections)
        if(connection->is_connected() && !poller->add_fd(connection->m_fd))
            throw EventException(EventExceptionT::PollerAddFailed);

    // Associate registered consumer to their events (so that
    // we can wake_up the condition_variable).
    // It throw EventCollision if failed.
    for (auto consumer : m_consumers)
    {
#define link(slot, link) link_consumer(consumer, consumer->slot, link);

        link(m_on_disconnect_slot, m_on_disconnect_link);
        link(m_on_server_disconnect_slot, m_on_server_disconnect_link);
        link(m_on_event_slot, m_on_event_link);

        for (auto slot_pair : consumer->m_slots)
            link_consumer(consumer, slot_pair.second, m_links[slot_pair.first]);
    }

    //Keep the event poll
    using std::swap;
    swap(m_poller, poller);
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
    for (auto s : m_servers)
    {
        if (s->get_fd() == fd)
        {
            //Create event and notify thread
            tell_disconnected(s);
            //Close the server
            close(fd);
            if (m_poller)
                m_poller->remove_fd(fd);
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
                  << "Lost a " << type << " disconnected event for fd "
                  << fd
                  << std::endl;
#endif /* !SEDNL_NOWARN */
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
    if (!cn)
    {
        auto it = m_internal_connections.find(fd);
        if (it != m_internal_connections.end())
        {
            cn = it->second;
            m_internal_connections.erase(it);
        }
    }

    if (!cn)
        return;

    cn->safe_disconnect();
    if (m_poller)
        m_poller->remove_fd(fd);

    //Create the server disconnected event
    disconnected_event(m_disconnected_queue, cn, cn->get_fd(), "connection");

    //Wake up consumer
    notify(m_on_disconnect_link);
}

//Assume fd is a server
void EventListener::accept_connections(FileDescriptor fd)
{
    while (true)
    {
#ifdef SEDNL_WINDOWS
        FileDescriptor cfd = accept(fd, nullptr, nullptr);
#else /* SEDNL_WINDOWS */
        sockaddr_in in_addr;
        int in_len = sizeof(in_addr);
        FileDescriptor cfd = accept(fd, (sockaddr*)&in_addr, &in_len);
#endif /* SEDNL_WINDOWS */
        if (cfd < 0)
        {
#ifndef SEDNL_WINDOWS
            //No more fd
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
#else /* !SEDNL_WINDOWS */
	    auto errc = WSAGetLastError();
            if (errc == WSAEWOULDBLOCK)
	        break;
#endif /* !SEDNL_WINDOWS */

#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't accept on server socket "
                      << fd
                      << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* !SEDNL_NOWARN */
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
#endif /* !SEDNL_NOWARN */
            close(cfd);
            return;
        }

        if (m_poller->add_fd(cfd) < 0)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: "
                      << "Can't accepted socket to epoll "
                      << cfd
                      << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* !SEDNL_NOWARN */
            close(cfd);
            return;
        }

        //Create the connection
        std::shared_ptr<Connection> connection;
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

            using std::swap;
            swap(connection, cn);
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
#endif /* !SEDNL_NOWARN */
            close(cfd);
            m_poller->remove_fd(cfd);
            return;
        }

        //Create the event
        try
        {
            if (m_on_connect_slot)
                m_on_connect_slot(*connection);
        }
        catch(std::exception& e)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: Non handled exception caugh inside on_connect."
                      << std::endl;
            std::cerr << "    "
                      << e.what()
                      << std::endl;
#endif /* !SEDNL_NOWARN */
        }
        catch(...)
        {
#ifndef SEDNL_NOWARN
            std::cerr << "Warning: Non handled exception caugh inside on_connect."
                      << std::endl;
#endif /* !SEDNL_NOWARN */
        }

        //We did it!
    }
}


//Assume fd is a connection
void EventListener::read_connection(FileDescriptor fd)
{
    char buf[512];
    ssize_t count = 0;
    Event e;
    std::shared_ptr<Connection> cn = get_connection(fd);

    while (true)
    {
      count = recv(fd, buf, sizeof(buf), 0);

        if (count == -1)
        {
            //End of stream
#ifndef SEDNL_WINDOWS
            if (errno == EAGAIN)
                break;
#else /* !SEDNL_WINDOWS */
	    auto errc = WSAGetLastError();
	    if (errc == WSAEWOULDBLOCK)
	      break;
#endif /* !SEDNL_WINDOWS */

            //Error
#ifndef SEDNL_NOWARN
            //This isn't a big error, and mayb we should
            // use NDEBUG instead of SEDNL_NOWARN.
            std::cerr << "warning: Reading data from connection "
                      << fd
                      << " failed." << std::endl;
            std::cerr << "    " << strerror(errno) << std::endl;
#endif /* !SEDNL_NOWARN */
            close_connection(fd);
            return;
        }
        //Connection closed
        else if (count == 0)
        {
            close_connection(fd);
            return;
        }

        //Push data in buffer
        cn->m_buffer.put(buf, static_cast<unsigned int>(count));

        //Try to read some events
        while (cn->m_buffer.pick_event(e))
        {
            if (!m_events[e.get_name()].push(std::make_pair(cn, e)))
            {
#ifndef SEDNL_NOWARN
                std::cerr << "Error: "
                          << "Lost a \"" << e.get_name()
                          << "\" event for fd "
                          << fd
                          << std::endl;
#endif /* !SEDNL_NOWARN */
            }
            else
            {
                if (m_links.find(e.get_name()) != m_links.end())
                    notify(m_links[e.get_name()]);
                else
                    notify(m_on_event_link);
            }
        }
    }
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

    if (m_poller)
        m_poller->remove_fd(ptr->m_fd);
    disconnected_event(m_disconnected_queue, ptr, ptr->m_fd, "connection");
}

//We are called with the m_fd lock
void EventListener::tell_disconnected(TCPServer *s) noexcept
{
    disconnected_event(m_server_disconnected_queue, s, s->get_fd(), "server");
    notify(m_on_server_disconnect_link);
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
        m_poller->wait_for_events(100);

        Poller::Event e;
        while (m_poller->next_event(e))
	  {
            //An error occured or the connection was closed
            if (e.is_close)
            {
                if (is_server(e.fd))
                    close_server(e.fd);
                else
                    close_connection(e.fd);
                continue;
            }

            //SHOULDN't HAPPEN! If it happens, we ignore it
            if (!e.is_read)
            {
#ifndef SEDNL_NOWARN
                std::cerr << "Warning: "
                          << "epoll_wait() returned a non EPOLLIN event."
                          << std::endl;
#endif /* !SEDNL_NOWARN */
                continue;
            }

            //So, it's a read event.

            //Incoming connection
            if (is_server(e.fd))
            {
                accept_connections(e.fd);
                continue;
            }
            //It's a read on a connection
            else
            {
                read_connection(e.fd);
                continue;
            }
        }
    }

    //Close all connections (and create events)
    for (auto cn : m_internal_connections)
        cn.second->disconnect();

    //Join consumer threads
    for (auto consumer : m_consumers)
        consumer->join();

    //Release resources
    m_poller.release();
    m_internal_connections.clear();
    clear_consumer_links();
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

void EventListener::remove_consumer(EventConsumer* c) noexcept
{
    if (m_running)
        throw EventException(EventExceptionT::EventListenerRunning);

    auto it = std::find(m_consumers.begin(),
                        m_consumers.end(),
                        c);
    if(it == m_consumers.end())
        return;
    m_consumers.erase(it);
}

void EventListener::add_consumer(EventConsumer* c) noexcept
{
    if (m_running)
        throw EventException(EventExceptionT::EventListenerRunning);
    auto it = std::find(m_consumers.begin(),
                        m_consumers.end(),
                        c);
    if (it == m_consumers.end())
        m_consumers.push_back(c);
}

void EventListener::notify(ConsumerDescriptor* desc) noexcept
{
    if (!desc)
        return;

    try
    {
        std::lock_guard<std::mutex> lk(desc->mutex);
        desc->wake_up = true;
        desc->cv.notify_one();
    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
        std::cerr << "Error: "
                  << "Failed to notify the thread described by "
                  << desc
                  << std::endl;
        std::cerr << "    " << e.what() << std::endl;
#endif /* !SEDNL_NOWARN */
    }
}

} // namespace SedNL
