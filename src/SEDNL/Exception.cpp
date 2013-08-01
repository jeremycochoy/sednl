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
    switch(m_type)
    {
    case TypeExceptionT::UserDataWrongTypeAsked:
        return "Tried to convert a data user to a wrong type.";
    case TypeExceptionT::UserDataWrongTypeGiven:
        return "Tried to write a data user from a different type"
            " thant the previous one stored.";
    default:
        return "Unknown exception.";
    }
}

template<>
const char* TemplateException<PacketExceptionT>::what() const noexcept
{
    switch(m_type)
    {
    case PacketExceptionT::UInt8Expected:
        return "The next element of this packet is an UInt8.";
    case PacketExceptionT::UInt16Expected:
        return "The next element of this packet is an UInt16.";
    case PacketExceptionT::UInt32Expected:
        return "The next element of this packet is an UInt32.";
    case PacketExceptionT::UInt64Expected:
        return "The next element of this packet is an UInt64.";
    case PacketExceptionT::Int8Expected:
        return "The next element of this packet is an Int8.";
    case PacketExceptionT::Int16Expected:
        return "The next element of this packet is an Int16.";
    case PacketExceptionT::Int32Expected:
        return "The next element of this packet is an Int32.";
    case PacketExceptionT::Int64Expected:
        return "The next element of this packet is an Int64.";
    case PacketExceptionT::FloatExpected:
        return "The next element of this packet is a Float.";
    case PacketExceptionT::DoubleExpected:
        return "The next element of this packet is a Double.";
    case PacketExceptionT::StringExpected:
        return "The next element of this packet is a String.";
    case PacketExceptionT::ObjectExpected:
        return "The next element of this packet is an Object.";

    case PacketExceptionT::ArrayUInt8Expected:
        return "The next element of this packet is an ArrayUInt8.";
    case PacketExceptionT::ArrayUInt16Expected:
        return "The next element of this packet is an ArrayUInt16.";
    case PacketExceptionT::ArrayUInt32Expected:
        return "The next element of this packet is an ArrayUInt32.";
    case PacketExceptionT::ArrayUInt64Expected:
        return "The next element of this packet is an ArrayUInt64.";
    case PacketExceptionT::ArrayInt8Expected:
        return "The next element of this packet is an ArrayInt8.";
    case PacketExceptionT::ArrayInt16Expected:
        return "The next element of this packet is an ArrayInt16.";
    case PacketExceptionT::ArrayInt32Expected:
        return "The next element of this packet is an ArrayInt32.";
    case PacketExceptionT::ArrayInt64Expected:
        return "The next element of this packet is an ArrayInt64.";
    case PacketExceptionT::ArrayFloatExpected:
        return "The next element of this packet is a ArrayFloat.";
    case PacketExceptionT::ArrayDoubleExpected:
        return "The next element of this packet is a ArrayDouble.";

    case PacketExceptionT::EmptyObject:
        return "The object you tried to read/write to/from the packet"
            "haven't any data field.";
    case PacketExceptionT::WrongSizedObject:
        return "The object stored in this packet doesn't mach the size"
            " of your object.";
    case PacketExceptionT::WrongArray:
        return "Empty array or array too big.";
    default:
        return "Unknown exception";
    }
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
