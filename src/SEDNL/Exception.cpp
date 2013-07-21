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

#include "SEDNL/Exception.hpp"

namespace SedNL
{

template<>
const char* TemplateException<NetworkExceptionT>::what() const noexcept
{
    switch(m_type)
    {
    case NetworkExceptionT::InvalidSocketAddress:
        return "The SocketAddress object given is invalid for it's purpose.";
    case NetworkExceptionT::CantRetrieveHost:
        return "Can't retrieve host ip from the host name.";
    case NetworkExceptionT::ConnectFailed:
        return "Can't connect to the server.";
    case NetworkExceptionT::PartialSend:
        return "A write to the socket failed after some bytes was writen."
            " The connection was closed.";
    case NetworkExceptionT::EmptySend:
        return "A write to the socket failed and the event wasn't sent."
            " The connection remain valid.";
    case NetworkExceptionT::SendFailed:
        return "A write to the socket failed with an error code.";
    case NetworkExceptionT::BindFailed:
        return "Can't bind the server socket.";
    case NetworkExceptionT::ListenFailed:
        return "Can't listen on the server socket.";
    case NetworkExceptionT::CantSetNonblocking:
        return "Can't set socket mode to nonblocking.";
    case NetworkExceptionT::TimedOut:
        return "Connection timed out.";
    default:
        return "Unknown exception.";
    }
}

template<>
const char* TemplateException<TypeExceptionT>::what() const noexcept
{
    return "TODO";
}

template<>
const char* TemplateException<EventExceptionT>::what() const noexcept
{
    switch(m_type)
    {
    case EventExceptionT::EventListenerRunning:
        return "An event lister was already running when you tried to change it's internal state. (calling set attach, detach, etc.)";
    case EventExceptionT::EventConsumerRunning:
        return "An event consumer was already running when your tried to change it's internal state. (calling set_producer, trying to change events bindings, etc.)";
    case EventExceptionT::EventCollision:
        return "Two or more EventConsumer binding the same event are attached to this listener.";
    case EventExceptionT::AlreadyListened:
        return "Tried to attach() an element to a listener that was"
            " already attached.";
    case EventExceptionT::WrongParentListener:
        return "The element on wich detach() was called wasn't attached"
            " to this listener.";
    case EventExceptionT::EpollCreateFailed:
        return "Failed to create the EPoll.";
    case EventExceptionT::EpollCtlFailed:
        return "Failed to register filedescriptors into epoll.";
    default:
        return "Unknown exception.";
    }
}

}
