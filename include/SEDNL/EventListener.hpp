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

#include "SEDNL/SocketHelp.hpp"

#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/ThreadHelp.hpp"
#include "SEDNL/Types.hpp"
#include "SEDNL/Event.hpp"
#include "SEDNL/Poller.hpp"

#include <queue>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>

namespace SedNL
{

class Connection;
class TCPServer;
class EventConsumer;
class Event;
class ConsumerDescriptor;
class Poller;

class SEDNL_API EventListener
{
private:
    typedef std::vector<EventConsumer*> ConsumerList;

public:
    //! \brief Construct an event listener from a TCPServer
    //!
    //! Does exactly the same thing as constructing an empty
    //! listener, and then calling attach(server).
    //!
    //! \param[in] server The server to listen
    //! \param[in] max_queue_size Upper bound of the size of an
    //!                           event queue.
    EventListener(TCPServer &server, unsigned int max_queue_size = 100);

    //! \brief Construct an event listener from a Connection
    //!
    //! Since TCPClient is a connection, you can also call
    //! EventListener(TCPConnection &connection).
    //!
    //! Does exactly the same thing as constructing an empty
    //! listener, and then calling attach(connection).
    //!
    //! \param[in] connection The connection to listen
    //! \param[in] max_queue_size Upper bound of the size of an
    //!                           event queue.
    EventListener(Connection &connection, unsigned int max_queue_size = 100);

    //! \brief Construct an empty listener.
    //!
    //! You can allow bigguer queue size by setting max_queue_size to
    //! an other value. You can suppress the upper bound by setting
    //! max_queue_size to 0, but it's not recommended.
    //!
    //! \param[in] max_queue_size Upper bound of the size of an
    //!                           event queue.
    EventListener(unsigned int max_queue_size = 100);

    //! \brief Add a server into the managed list
    //!
    //! Same behavior as attach() on a Connection object.
    //!
    //! \param[in] server The server to attach
    void attach(TCPServer &server) throw(std::bad_alloc, EventException);

    //! \brief Remove a server from the managed list
    //!
    //! Same behavior as detach() on a Connection object.
    //!
    //! \param[in] server The server to detach
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
    //! \param[in] connection The connection to attach
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
    //! \param[in] connection The connection to attach
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
    //! If more than one attached consumer bind the same event, then
    //! it will throw a EventCollision exception.
    //!
    //! To stop the listener, call join().
    void run() throw(EventException);

    //! \brief Join the EventListener thread
    //!
    //! When you call join, you ask the EventListener to stop
    //! processing events.
    //!
    //! When join() is called, The EventListener close all the
    //! internal connections.
    //! Then, it join each consumer.
    //! When all consumer are stoped, the join function return.
    //! It means that each disconnected event will be processed.
    void join();

private:
    //! \brief The EventListener thread's main function.
    void run_imp();

    //! \brief Maximal size of a queue.
    unsigned int m_max_queue_size;

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
    typedef std::map<FileDescriptor, std::shared_ptr<Connection>> InternalList;

    typedef std::pair<std::shared_ptr<Connection>, Event> CnEvent;
    typedef SafeQueue<CnEvent> EventQueue;
    typedef SafeQueue<std::shared_ptr<Connection>> ConnectionQueue;
    typedef SafeQueue<TCPServer *> ServerQueue;
    typedef std::map<std::string, EventQueue> EventMap;

    //! \brief The 'connected' event queue
    ConnectionQueue m_connected_queue;
    //! \brief The 'disconnected' event queue
    ConnectionQueue m_disconnected_queue;
    //! \brief The 'server disconnected' queue
    ServerQueue m_server_disconnected_queue;

    //! \brief The map of all event queue
    EventMap m_events;

    //
    // We assert that each list contain only one
    // time the same object.
    //
    // It SHOULD always be true.
    //

    ServerList m_servers;
    ConnectionList m_connections;
    InternalList m_internal_connections;

    // --------------------------------------------------
    //
    // Implementation of the event listener loop
    //
    // --------------------------------------------------

    //! brief Initialise a lot of stuff before launching the thread
    void run_init();

    //! \brief Tell if a FileDescriptor is a server
    //!
    //! If it's false, it means it's a client.
    bool is_server(FileDescriptor fd) const noexcept;

    //! \brief Close a server and create the server_closed event
    void close_server(FileDescriptor fd);

    //! \brief Close a connection, and delete it if it's an internal one
    void close_connection(FileDescriptor fd);

    //! \brief Accept incoming connection on the server fd
    void accept_connections(FileDescriptor fd);

    //! \brief Read data (or close) from the connection fd
    void read_connection(FileDescriptor fd);

    //! \brief Return the TCPServer associated, or nullptr
    TCPServer* get_server(FileDescriptor fd) noexcept;

    //! \brief Return the Connection associated, or an empty shared_ptr.
    std::shared_ptr<Connection> get_connection(FileDescriptor fd) noexcept;

    //! \brief Called by a client when disconnected by disconnect()
    //!
    //! Called with cn->m_mutex locked!
    void tell_disconnected(Connection *cn) noexcept;

    //! \brief Called by a client when disconnected by disconnect()
    //!
    //! Called with cn->m_mutex locked!
    void tell_disconnected(TCPServer *cn) noexcept;

    // ----------------------------------------
    //
    // Implementation of consumers management
    //
    // ----------------------------------------

    //! \brief List of consumers attached
    ConsumerList m_consumers;

    // Links to consumers (by event)
    ConsumerDescriptor* m_on_connect_link;
    ConsumerDescriptor* m_on_disconnect_link;
    ConsumerDescriptor* m_on_server_disconnect_link;
    ConsumerDescriptor* m_on_event_link;
    typedef std::map<std::string, ConsumerDescriptor*> DescriptorMap;
    DescriptorMap m_links;

    //! \brief Add links to the consumer descriptor for the right events
    //!
    //! Look into the consumer if it has a non empty slot \a slot, and then link
    //! him into \a link.
    template<typename S>
    void link_consumer(EventConsumer* consumer, S& slot, ConsumerDescriptor*& link)
    throw(EventException);

    //! \brief Wake up a consumer using the condition variable
    //!
    //! If \a desc is nullptr, nothing is done.
    //!
    void notify(ConsumerDescriptor* desc) noexcept;

    //! \brief Remove all consumer links
    void clear_consumer_links() noexcept;

    //! \brief Called by EventConsumer's set_producer/remove_producer
    void remove_consumer(EventConsumer*) noexcept;

    //! \brief Called by EventConsumer's set_producer
    void add_consumer(EventConsumer*) noexcept;

    std::unique_ptr<Poller> m_poller;

    friend class Connection;
    friend class TCPServer;
    friend class EventConsumer;
};

} // namespace SedNL

#endif /* !EVENT_LISTENER_HPP_ */
