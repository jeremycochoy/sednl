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

#ifndef EVENT_LISTENER_HPP_
#define EVENT_LISTENER_HPP_

#ifndef EPOLL_SIZE
# define EPOLL_SIZE 10000
#endif /* !EPOLL_SIZE */
#ifndef MAX_EVENTS
# define MAX_EVENTS 256
#endif /* !MAX_EVENTS */

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <sys/epoll.h>

#endif /* SEDNL_WINDOWS */

#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/ThreadHelp.hpp"
#include "SEDNL/Types.hpp"

#include <queue>
#include <list>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>

namespace SedNL
{

class Connection;
class TCPServer;
class EventConsumer;
class Event;

class SEDNL_API EventListener
{
private:
    //! \brief Contain a consumer (and relatex mutex/cv)
    struct ConsumerDescriptor
    {
        std::mutex mutex;
        std::condition_variable cv;
        EventConsumer *consumer;
    };

    //! \brief Contain an event queue and the consumer associated
    struct EventDescriptor
    {
        std::queue<Event> queue;
        ConsumerDescriptor *desc;
    };

    typedef std::vector<ConsumerDescriptor> ConsumerList;

public:
    //! \brief Construct an event listener from a TCPServer
    //!
    //! Does exactly the same thing as constructing an empty
    //! listener, and then calling attach(server).
    //!
    //! \argument[in] server The server to listen
    //! \argument[in] max_queue_size Upper bound of the size of an
    //!                              event queue.
    EventListener(TCPServer &server, unsigned int max_queue_size = 100);

    //! \brief Construct an event listener from a Connection
    //!
    //! Since TCPClient is a connection, you can also call
    //! EventListener(TCPConnection &connection).
    //!
    //! Does exactly the same thing as constructing an empty
    //! listener, and then calling attach(connection).
    //!
    //! \argument[in] connection The connection to listen
    //! \argument[in] max_queue_size Upper bound of the size of an
    //!                              event queue.
    EventListener(Connection &connection, unsigned int max_queue_size = 100);

    //! \brief Construct an empty listener.
    //!
    //! You can allow bigguer queue size by setting max_queue_size to
    //! an other value. You can suppress the upper bound by setting
    //! max_queue_size to 0, but it's not recommended.
    //!
    //! \argument[in] max_queue_size Upper bound of the size of an
    //!                              event queue.
    EventListener(unsigned int max_queue_size = 100);

    //! \brief Add a server into the managed list
    //!
    //! Same behavior as attach() on a Connection object.
    //!
    //! \argument[in] server The server to attach
    void attach(TCPServer &server) throw(std::bad_alloc, EventException);

    //! \brief Remove a server from the managed list
    //!
    //! Same behavior as detach() on a Connection object.
    //!
    //! \argument[in] server The server to detach
    void detach(TCPServer &server) throw(std::bad_alloc, EventException);


    //! \brief Add a connection into the managed list
    //!
    //! The client will be kept by the EventListener. The object
    //! shouldn't be destructed before the EventLister.
    //! If you want to destruct it, you need to remove it from
    //! the "managed list" by calling detach().
    //!
    //! A connection cannot be attached to two listener.
    //! Try to do it will result in an AlreadyListenned exception.
    //!
    //! You can't call attach while the listener is runing.
    //! If you do so, it will through a EventListenerRunning
    //! exception.
    //!
    //! Calling attach on a managed connection won't do anything.
    //!
    //! \argument[in] connection The connection to attach
    void attach(Connection &connection) throw(std::bad_alloc, EventException);

    //! \brief Remove a connection from the managed list
    //!
    //! Calling detach with an unmanaged connection won't
    //! do anything.
    //!
    //! Detaching from the wrong listener will result in an
    //! WrongParentListener exception.
    //!
    //! You can't call detach while the listener is runing.
    //! If you do so, it will throw a EventListenerRunning
    //! exception.
    //!
    //! \argument[in] connection The connection to attach
    void detach(Connection &connection) throw(std::bad_alloc, EventException);

    //! \brief Launch the EventListener thread
    //!
    //! The EventListener launch a new thread and start listening to
    //! incoming events from Connection and TCPServer.
    //!
    //! If a new connection occure from TCPServer, a new Connection is
    //! created and added into the connection list.
    //!
    //! The EventListener will take care of the memory allocated by new
    //! connections.
    //!
    //! To stop the listener, call join().
    void run() throw(EventException);

    //! \brief Join the EventListener thread
    //!
    //! When you call join, you ask the EventListener to stop
    //! processing events.
    //!
    //! When join() is called, The EventListener close all the connections.
    //! Then, it join each consumer.
    //! When all consumer are stoped, the join function return.
    //! It means that each disconnected event will be processed.
    void join();

private:
    //! \brief The EventListener thread's main function.
    void run_imp();

    //! \brief Maximal size of a queue.
    unsigned int m_max_queue_size;

    //! \brief List of consumers attached
    ConsumerList m_consumers;

    //! \brief The EventListener thread
    std::thread m_thread;

    //! \brief The running state
    SafeType<bool> m_running;

    //! \brief Internal implementation of attach
    template<class L, class T> inline
    void __attach(L& list, T *elm);

    //! \brief Internal implementation of detach
    template<class L, class T> inline
    void __detach(L& list, T *elm);

    typedef std::vector<TCPServer*> ServerList;
    typedef std::vector<Connection*> ConnectionList;
    typedef std::list<std::shared_ptr<Connection>> InternalList;

    //
    // We assert that each list contain only one
    // time the same object.
    //
    // It SHOULD always be true.
    //

    ServerList m_servers;
    ConnectionList m_connections;
    InternalList m_internal_connections;

    //! brief Initialise a lot of stuff before launching the thread
    void run_init();

    //! \brief Tell if a FileDescriptor is a server
    bool is_server(FileDescriptor fd);

    //! \brief Close a server and create the server_closed event
    void close_server(FileDescriptor fd);

    //! \brief Close a connection, and delete it if it's an internal one
    void close_connection(FileDescriptor fd);

    //! \brief Return the TCPServer associated, or nullptr
    TCPServer* get_server(FileDescriptor fd);

    FileDescriptor m_epoll;
    std::unique_ptr<struct epoll_event[]> m_events;
};

} // namespace SedNL

#endif /* !EVENT_LISTENER_HPP_ */
