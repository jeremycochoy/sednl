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

#include "SEDNL/sednlfwd.hpp"
#include "SEDNL/SocketHelp.hpp"
#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/ThreadHelp.hpp"
#include "SEDNL/Types.hpp"
#include "SEDNL/Event.hpp"
#include "SEDNL/Poller.hpp"
#include "SEDNL/Slot.hpp"

#include <queue>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>

namespace SedNL
{

////////////////////////////////////////////////////////////
//! \brief Listen from events and incoming connections.
////////////////////////////////////////////////////////////
class SEDNL_API EventListener
{
private:
    typedef std::vector<EventConsumer*> ConsumerList;

public:
    //! \brief Construct an event listener from a TCPServer.
    //!
    //! Does exactly the same thing as constructing an empty
    //! listener, and then calling attach(server).
    //!
    //! \param[in] server The server to listen.
    //! \param[in] max_queue_size Upper bound of the size of an
    //!                           event queue.
    EventListener(TCPServer &server, unsigned int max_queue_size = 1000);

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
    EventListener(Connection &connection, unsigned int max_queue_size = 1000);

    //! \brief Construct an empty listener.
    //!
    //! You can allow bigguer queue size by setting max_queue_size to
    //! an other value. You can suppress the upper bound by setting
    //! max_queue_size to 0, but it's not recommended.
    //!
    //! \param[in] max_queue_size Upper bound of the size of an
    //!                           event queue.
    EventListener(unsigned int max_queue_size = 1000);

    //! \brief Destruct the event listener by calling join.
    ~EventListener();

    //! \brief Add a server into the managed list.
    //!
    //! Same behavior as attach() on a Connection object.
    //!
    //! \param[in] server The server to attach.
    void attach(TCPServer &server) throw(std::bad_alloc, EventException);

    //! \brief Remove a server from the managed list.
    //!
    //! Same behavior as detach() on a Connection object.
    //!
    //! \param[in] server The server to detach.
    void detach(TCPServer &server) throw(std::bad_alloc, EventException);


    //! \brief Add a connection into the managed list.
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
    //! \param[in] connection The connection to attach.
    void attach(Connection &connection) throw(std::bad_alloc, EventException);

    //! \brief Remove a connection from the managed list.
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
    //! \param[in] connection The connection to detach.
    void detach(Connection &connection) throw(std::bad_alloc, EventException);

    //! \brief Launch the EventListener thread.
    //!
    //! EventListener::run() launch a new thread and start listening to
    //! incoming events from TCPConnection, Connection and TCPServer.
    //!
    //! If a new connection occure from TCPServer, a new Connection is
    //! created and added into the connection list.
    //!
    //! The EventListener will take care of the memory allocated by new
    //! connections, and memory desalocation.
    //!
    //! If more than one attached consumer bind the same event, then
    //! it will throw a EventCollision exception at startup.
    //!
    //! To stop the listener, call join().
    void run() throw(EventException);

    //! \brief Join the EventListener thread.
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

    //! \brief Bind the _connect_ event.
    //!
    //! Callback prototype : void my_on_connect(Connection&);
    //!
    //! The _connect_ event is guaranted to be processed imediately when
    //! a connection is received from the server.
    //!
    //! Since the callback is executed in the listener thread,
    //! you would like to keep it small and fast.
    inline Slot<Connection&>& on_connect();

private:
    //! \brief the connection event slot.
    Slot<Connection&> m_on_connect_slot;

    //! \brief The EventListener thread's main function.
    void run_imp();

    //! \brief Maximal size of a queue.
    unsigned int m_max_queue_size;

    //! \brief The EventListener thread.
    std::thread m_thread;

    //! \brief The running state.
    SafeType<bool> m_running;

    //! \brief Internal implementation of attach.
    template<class L, class T> inline
    void __attach(L& list, T *elm);

    //! \brief Internal implementation of detach.
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

    //! \brief The 'disconnected' event queue.
    ConnectionQueue m_disconnected_queue;
    //! \brief The 'server disconnected' queue.
    ServerQueue m_server_disconnected_queue;

    //! \brief The map of all event queue.
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
    // Implementation of the event listener loop.
    //
    // --------------------------------------------------

    //! brief Initialise a lot of stuff before launching the thread.
    void run_init();

    //! \brief Tell if a FileDescriptor is a server.
    //!
    //! If it's false, it means it's a client.
    bool is_server(FileDescriptor fd) const noexcept;

    //! \brief Close a server and create the server_closed event.
    void close_server(FileDescriptor fd);

    //! \brief Close a connection, and delete it if it's an internal one.
    void close_connection(FileDescriptor fd);

    //! \brief Accept incoming connection on the server fd.
    void accept_connections(FileDescriptor fd);

    //! \brief Read data (or close) from the connection fd.
    void read_connection(FileDescriptor fd);

    //! \brief Return the TCPServer associated, or nullptr.
    TCPServer* get_server(FileDescriptor fd) noexcept;

    //! \brief Return the Connection associated, or an empty shared_ptr.
    std::shared_ptr<Connection> get_connection(FileDescriptor fd) noexcept;

    //! \brief Called by a client when disconnected by disconnect().
    //!
    //! Called with cn->m_mutex locked!
    void tell_disconnected(Connection *cn) noexcept;

    //! \brief Called by a client when disconnected by disconnect().
    //!
    //! Called with cn->m_mutex locked!
    void tell_disconnected(TCPServer *cn) noexcept;

    // ----------------------------------------
    //
    // Implementation of consumers management.
    //
    // ----------------------------------------

    //! \brief List of consumers attached.
    ConsumerList m_consumers;

    // Links to consumers (by event).
    ConsumerDescriptor* m_on_disconnect_link;
    ConsumerDescriptor* m_on_server_disconnect_link;
    ConsumerDescriptor* m_on_event_link;
    typedef std::map<std::string, ConsumerDescriptor*> DescriptorMap;
    //Do not use [] with m_links if you don't want to add a new link.
    //Use m_links.find().
    DescriptorMap m_links;

    //! \brief Add links to the consumer descriptor for the right events.
    //!
    //! Look into the consumer if it has a non empty slot \a slot, and then link
    //! him into \a link.
    template<typename S>
    void link_consumer(EventConsumer* consumer, S& slot, ConsumerDescriptor*& link)
    throw(EventException);

    //! \brief Wake up a consumer using the condition variable.
    //!
    //! If \a desc is nullptr, nothing is done.
    //!
    void notify(ConsumerDescriptor* desc) noexcept;

    //! \brief Remove all consumer links.
    void clear_consumer_links() noexcept;

    //! \brief Called by EventConsumer's set_producer/remove_producer.
    void remove_consumer(EventConsumer*) noexcept;

    //! \brief Called by EventConsumer's set_producer.
    void add_consumer(EventConsumer*) noexcept;

    std::unique_ptr<Poller> m_poller;

    friend class Connection;
    friend class TCPServer;
    friend class EventConsumer;
};

} // namespace SedNL

#include "EventListener.ipp"

#endif /* !EVENT_LISTENER_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::EventListener
//!
//! The EventListener class is one of the central class of
//! SedNL. You should use it on 'server side' to listen from
//! incoming connections, and event from those connections.
//!
//! You should also use it to listen from events from
//! TCPClient connections.
//!
//! You can give as much TCPClient and TCPServer objects as
//! you want to the same EventListener, it will handle all of
//! them.
//!
//! When a new connection is created, it produce a _connect_
//! event which is immediately processed (which means
//! that the listener thread will call the callback you put
//! on the slot returned by on_connect()).
//!
//! Then, every event comming from TCPConnection and Connections
//! will be placed in queue (one queue by event name, placed
//! in a deterministic order).
//!
//! The next step is creating one or more EventConsumer
//! that would be able to consume events stored in queue.
//! This allow you to manage how much thread you want, and
//! which events should be processed at the same time.
//! For more information on this subject, see EventConsumer's
//! documentation.
//!
//! Here is a short example of what you can do with
//! an EventListener.
//!
//! \code
//!
//! //Connect to two different servers
//! TCPClient client1(SocketAddress("foo.net", 4242));
//! TCPClient client2(SocketAddress("bar.net", 4343));
//!
//! //Listen to two different port
//! TCPServer server1(SocketAddress(5000));
//! TCPServer server2(SocketAddress(8000));
//!
//! //Create an empty EventListener.
//! EventListener listener;
//!
//! //Listen messages from foo and bar
//! listener.attach(client1);
//! listener.attach(client2);
//!
//! //Listen from incoming connection in both port 5000 and port 8000.
//! //We do not differenciate connection from different ports.
//! listener.attach(server1);
//! listener.attach(server2);
//!
//! //When a new user connect from server1 or server2.
//! //Lets says we allocate some memory to store informations
//! //related to this connection, and use Connection::set_user_data().
//! listener.on_connect(my_on_connect);
//!
//! //We want to react on message from foo.net, bar.net, and users
//! //connected to port 5000 and 8000.
//! EventConsumer consumer(listener);
//! consumer.bind("on_hello").set_function(my_hello_stuff);
//!
//! //We can imagine that we forward messages to foo.net or
//! //bar.net depending of the request.
//! consumer.bind("on_query_foo").set_function(my_query_foo_stuff);
//! consumer.bind("on_query_bar").set_function(my_query_bar_stuff);
//! consumer.bind("answer_from_bar").set_function(my_bar_answer_stuff);
//! consumer.bind("answer_from_foo").set_function(my_foo_answer_stuff);
//!
//! //We are ready, and launch the listener thread
//! listener.run();
//!
//! //We also launch the consumer thread
//! consumer.run();
//!
//! //Now we can do what we want in the main thread.
//! //for example, providing commands from stdin.
//!
//! //When we are done, we should close all connections.
//! listener.join();
//! consumer.join();
//!
//! \endcode
//!
//! If in this example, you would need to differenciate messages from
//! foo and messages from bar, you could check the address of the first
//! parameter of you callback (Connection&).
//! It is always a reference to the TCPClient object you given, casted to
//! a Connection object.
//!
//! So, you would do something like :
//!
//! \code
//!
//! void on_something(Connection& c, const Event& e)
//! {
//!     if(static_cast<Connection*>(&client1) == &c)
//!         std::cout << "It's from foo." << std::endl;
//!     if(static_cast<Connection*>(&client2) == &c)
//!         std::cout << "It's from bar." << std::endl;
//! }
//!
//! \endcode
////////////////////////////////////////////////////////////
