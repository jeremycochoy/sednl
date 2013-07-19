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

#include "SEDNL/EventConsumer.hpp"
#include "SEDNL/EventListener.hpp"
#include "SEDNL/Exception.hpp"

namespace SedNL
{

EventConsumer::EventConsumer()
    :m_producer(nullptr), m_running(false)
{}

EventConsumer::EventConsumer(EventListener &producer)
    :EventConsumer()
{
    set_producer(producer);
}

void EventConsumer::remove_producer() noexcept
{
    if (m_running)
        throw EventException(EventExceptionT::EventConsumerRunning);

    if (m_producer)
    {
        m_producer->remove_consumer(this);
        m_producer = nullptr;
    }
}

void EventConsumer::set_producer(EventListener &producer) throw(EventException)
{
    if (m_running)
        throw EventException(EventExceptionT::EventConsumerRunning);

    if (m_producer)
        m_producer->remove_consumer(this);
    m_producer = &producer;
    m_producer->add_consumer(this);
}

} //namespace SedNL

