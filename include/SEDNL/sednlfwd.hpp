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

#ifndef SEDNL_FWD_HPP_
#define SEDNL_FWD_HPP_

namespace SedNL
{

//! @cond Doxygen_Suppress

class TCPClient;
class TCPServer;
class Connection;
class EventListener;
class EventConsumer;
class Packet;
class PacketReader;
class RingBuf;
class Event;
class SocketAddress;
class SocketInterface;
class ConsumerDescriptor;
class Exception;
template<typename T>
class TemplateException;
class Poller;
template<class T>
struct Serializer;
template<class T>
class SafeType;
template<class T, class Container>
class SafeQueue;

//! @endcond

} //namespace SedNL

#endif /* SEDNL_FWD_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \file sednlfwd.hpp
//! \brief Forward declaration of SedNL objects.
//!
//! This file contain forward declaration of SedNL's classes
//! and structs.
//!
////////////////////////////////////////////////////////////
