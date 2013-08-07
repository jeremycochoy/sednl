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

class EventListener;
class Connection;
class Event;
class TCPServer;

//! \brief Contain a consumer mutex/cv
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
    //! \brief Construct a consumer
    EventConsumer();

    //! \brief Construct a consumer consuming events
    //!        from \a producer.
    //!
    //! Same effet as calling the empty constructor and then calling
    //! set_producer().
    //!
    //! \param[in] producer The event listener from which events will
    //!                     be consumed.
    EventConsumer(EventListener &producer);

    //! \brief Set the producer
    //!
    //! The \a producer shoulde live until the EventConsumer die.
    //! (or, at least, until the EventConsumer stop to use producer,
    //!  which means that once the producer is destroyed,
    //!  you shouldn't call run, and the consumer shouldn't be running.)
    //!
    //! Setting the same producer to two consumer that bind the same
    //! event will result in a deffered throw : the call to producer's run
    //! will throw an EventException with code EventCollision.
    //!
    //! \param[in] producer The event listener from which events will
    //!                     be consumed.
    void set_producer(EventListener& producer) throw(EventException);

    //! \brief Remove the previous producer
    //!
    //! See set_producer(EventListener&) documentation.
    void remove_producer() noexcept;

    //! \brief Start the consumer thread
    void run() throw(EventException);

    //! \brief Join the consumer thread, and stop consuming events.
    void join();

    //! \brief Bind the disconnect event
    inline Slot<Connection&>& on_disconnect();

    //! \brief Bind the server disconnect event
    inline Slot<TCPServer&>& on_server_disconnect();

    //! \brief Bind all unbinded events
    //!
    //! It will be called for events that aren't consumed
    //! by any consumer.
    //!
    //! You can bind this event to consume all events not binded by
    //! EventConsumer::bind(). You can use use others consumer with the
    //! same EventListener producer, but there should be only one
    //! providing a on_event() slot.
    inline Slot<Connection&, const Event&>& on_event();

    //! \brief Bind a new event
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

    //! \brief Remove empty slots from the map
    void clean_slots();

    //! \brief Consume events available from producer
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
//!
//! You can't bind new events while a consumer is running. If you wan't to bind new
//! events, you'll have to join the EventConsumer thread, then add new events, and
//! finaly start it again.
//!
////////////////////////////////////////////////////////////

