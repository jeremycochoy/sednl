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

#include "SEDNL/Export.hpp"

#include <queue>
#include <vector>
#include <condition_variable>
#include <mutex>

namespace SedNL
{

class Connection;
class TCPServer;
class EventConsumer;

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
    void attach(TCPServer &server);

    //! \brief Remove a server from the managed list
    //!
    //! Same behavior as detach() on a Connection object.
    //!
    //! \argument[in] server The server to detach
    void detach(TCPServer &server);


    //! \brief Add a connection into the managed list
    //!
    //! The client will be kept by the EventListener. The object
    //! shouldn't be destructed before the EventLister.
    //! If you want to destruct it, you need to remove it from
    //! the "managed list" by calling detach().
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
    //! You can't call detach while the listener is runing.
    //! If you do so, it will through a EventListenerRunning
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
    void run();

    //! \brief Join the EventListener thread
    //!
    //! When you call join, you ask the EventListener to stop
    //! processing events.
    //!
    //! When join() is called, The EventListener close all the connections.
    //! Then, it join each consumer.
    //! When all consumer are stoped, the join function return.
    void join();

private:
    //! \brief Maximal size of a queue.
    unsigned int m_max_queue_size;

    //! \brief List of consumers attached
    ConsumerList m_consumers;
};

} // namespace SedNL

#endif /* !EVENT_LISTENER_HPP_ */
