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

#ifndef EVENT_CONSUMER_HPP_
#define EVENT_CONSUMER_HPP_

#include "SEDNL/sednlfwd.hpp"
#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/Types.hpp"
#include "SEDNL/Slot.hpp"
#include "SEDNL/ThreadHelp.hpp"

#include <unordered_map>
#include <condition_variable>
#include <thread>
#include <mutex>

namespace SedNL
{

//! \brief Contain a consumer mutex / condition variable.
//!
//! For implementation purpose.
class ConsumerDescriptor
{
public:
    ConsumerDescriptor()
        :wake_up(false)
    {};
    std::mutex mutex;
    bool wake_up;
    std::condition_variable cv;
};

template<typename... Args>
using SlotMap = std::unordered_map<std::string, Slot<Args...>>;

////////////////////////////////////////////////////////////////////
//! \brief A consumer object. It consume certain kind of events from
//!        an EventListener.
////////////////////////////////////////////////////////////////////
class SEDNL_API EventConsumer
{
public:
    //! \brief Construct a consumer.
    EventConsumer();

    //! \brief Construct a consumer consuming events from \a producer.
    //!
    //! Same effet as calling the empty constructor and then calling
    //! set_producer().
    //!
    //! \param[in] producer The event listener from which events will
    //!                     be consumed.
    EventConsumer(EventListener &producer);

    //! \brief Destruct the consumer by calling join.
    ~EventConsumer();

    //! \brief Set the producer.
    //!
    //! The \a producer shoulde live until the EventConsumer die
    //! (or, at least, until the EventConsumer stop to use \a producer,
    //!  which means that once the producer is destroyed,
    //!  you shouldn't call run, and the consumer shouldn't be running).
    //!
    //! Setting the same producer to two consumer that bind the same
    //! event will result in a deffered throw : the call to producer's run
    //! will throw an EventException with code EventCollision.
    //!
    //! \param[in] producer The event listener from which events will
    //!                     be consumed.
    void set_producer(EventListener& producer) throw(EventException);

    //! \brief Remove the previous producer.
    //!
    //! See set_producer(EventListener&) documentation.
    void remove_producer() noexcept;

    //! \brief Start the consumer thread.
    //!
    //! Once launcher, the consumer will read events from
    //! producer's queues. It will only consume events that
    //! are bound by this consumer.
    //!
    //! Notice that on_event() allow binding all events not
    //! already bound by a consumer (this one or an other one).
    void run() throw(EventException);

    //! \brief Join the consumer thread, and stop consuming events.
    void join();

    //! \brief Bind the _disconnect_ event.
    //!
    //! Callback prototype : void my_on_disconnect(Connection&);
    //!
    //! Warning: A disconnect event can be computed before
    //! an other event from the same client!
    //! This is a consequence of asynchronous nature of
    //! a multithreaded environment :
    //! nothing prevent the listener to push a new event and
    //! a disconnect event while we are processing
    //! all disconnect event available. And nothing
    //! prevent an other consumer thread to be processing
    //! an other event while we start processing
    //! a disconnect event.
    //! It was experimentaly saw that some time,
    //! disconnect event occure before some other
    //! events. You are warned! If you wan't to
    //! free resources when you receive a _disconnected_
    //! event, you'll have to always check if this resource
    //! is available, and remember that it can be destroyed
    //! at any time if you have more than one consumer
    //! thread.
    inline Slot<Connection&>& on_disconnect();

    //! \brief Bind the server disconnect event.
    //!
    //! Callback prototype : void my_on_server_disconnect(TCPServer&);
    //!
    //! This event only happen when the server socket is closed,
    //! which is ofted caused by you closing the socket, or network drop.
    //!
    //! The same warnings as on_disconnect() apply.
    //! You CAN'T assume that this event will be the last event
    //! processed from this server. You may see incoming connection
    //! after this event was processed.
    inline Slot<TCPServer&>& on_server_disconnect();

    //! \brief Bind all unbinded events.
    //!
    //! This callback will be called for every events that isn't
    //! binded by any of the consumers linked to the producer.
    //!
    //! You can bind this slot to consume all events not binded by
    //! EventConsumer::bind(). You can use use others consumer with the
    //! same EventListener producer, but there should be only one
    //! providing a on_event() callback. Otherwise, You'll have
    //! an 'event collision' when starting the producer (see
    //! EventListener::run()).
    inline Slot<Connection&, const Event&>& on_event();

    //! \brief Bind a new event.
    //!
    //! This function allow binding an event from it's name.
    //! Any Event object is constructed with a name, and this name is
    //! a string which identify the event.
    //! If you bind "foo" with bind("foo").set_function(my_callback),
    //! any event with the name "foo" will be dispatched to the callback
    //! my_callback.
    //!
    //! \param[in] event_name Name of the event that will be associated with
    //!                       this callback.
    inline Slot<Connection&, const Event&>& bind(std::string event_name);

private:
    ConsumerDescriptor m_descriptor;

    // Member data
    EventListener* m_producer;
    Slot<Connection&> m_on_disconnect_slot;
    Slot<TCPServer&> m_on_server_disconnect_slot;
    Slot<Connection&, const Event&> m_on_event_slot;
    SlotMap<Connection&, const Event&> m_slots;

    std::thread m_thread;

    SafeType<bool> m_running;

    //
    // Consumer implementation
    //

    void run_init();
    void run_imp();

    //! \brief Remove empty slots from the map.
    void clean_slots();

    //! \brief Consume events available from producer.
    void consume_events() noexcept;

    friend class EventListener;
};

} // namespace SedNL

#include "SEDNL/EventConsumer.ipp"

#endif /* !EVENT_CONSUMER_HPP_ */

////////////////////////////////////////////////////////////
//! \class SedNL::EventConsumer
//!
//! The EventConsumer consume events from an EventListener object.
//! It consume only event that can be processed, i.e. events that
//! have a callback associated (with a call to EventConsumer::bind()).
//!
//! It runs on it's own thread, using EventConsumer::run(). The thread can
//! be joined with EventConsumer::join().
//!
//! The main idea is to have one EventConsumer for a bunch of events, and one
//! more for an over bunch, etc.
//!
//! Two EventConsumer CAN'T consume the same type of events from a common
//! EventListener. If you try to do so, the EventConsumer::run()
//! call will throw an exception, telling that the event are already looked
//! from an other EventConsumer.
//! This is done to force consistency (nothing would prevent two consumer to do
//! different things for the same event).
//!
//! You can't bind new events while a consumer is running. If you wan't to bind new
//! events, you'll have to join the EventConsumer thread, then add new events, and
//! finaly start it again.
//!
//! Take a particular care about the behavior of the _disconnected_ event.
//! You CAN'T assume it will be the last event processed, even if you have
//! only one thread. See on_disconnect() documentation.
//!
//! Here is an example on how EventConsumer allow you to control
//! how much thread you wan't to allow for processing events.
//!
//! \code
//!
//! //Some callbacks.
//! void computation_intensive(Connection&, const Event&);
//! void loggin(Connection&, const Event&);
//! void easy_query(Connection&, const Event&);
//! void lots_of_calls(Connection&, const Event&);
//!
//! //We assume we have an EventListener named listener.
//!
//! //Assume we have a computation intensive query.
//! EventConsumer intensive(listener);
//! intensive.bind("travelling_selsman").set_function(computeation_intensive);
//!
//! //And an event that is received hundread of times by second
//! EventConsumer repeated(listener);
//! repeated.bind("time").set_function(lost_of_calls);
//!
//! //And all remaining events aren't critical
//! EventConsumer remaining(listener);
//! remaining.bind("loggin").set_function(loggin);
//! remaining.bind("echo").set_function(easy_query);
//!
//! //Then, we can launch three threads to consume those events,
//! //and a the listener thread.
//! listener.run();
//! intensive.run();
//! repeated.run();
//! remaining.run();
//!
//! //And continue our work in the main thread.
//!
//! //Once we are done, we join all those threads.
//! listener.join();
//! intensive.join();
//! repeated.join();
//! remaining.join();
//!
//! //Nb : It's better to always join the listener first.
//! //     If you do so, you are sure that all received
//! //     events are processed.
//!
//! \endcode
//!
////////////////////////////////////////////////////////////

