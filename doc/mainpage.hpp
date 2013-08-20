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

//! \mainpage
//!
//! \section summary Summary
//! Welcome to the official SedNL documentation. You will find
//! detailed description of classes and functions provided by the
//! library, with short examples.
//!
//! \section description Description
//! SedNL (for Simple Event Driven Network Library) is a C++11 library
//! that provide a simple hight level API to send and receive easily
//! events through network.
//!
//! An event is a string (the event name) followed by some data.
//! You build your own events easily, and can serialize instances.
//! You easily bind callback (lambda, functor, function or member function)
//! to events, and don't have to worry too much on how to queue events
//! and handle concurency.
//!
//! This library is intended for application that need to send events
//! that should'nt be lost, and should always be received in the same order they
//! was send (althought they will be processed asynchronously).
//! Basicaly, online games.
//! For this purpose, the library rely on TCP.
//!
//! This library was developed to provide a reliable and simple API
//! for a turn by turn MMORPG. By reliability, we means that event if
//! the os can't provide more memory, the behavior of the server is still
//! well defined, and it won't crash.
//!
//! \section tcp_udp TCP or UDP ?
//!
//! If you need the ability to lost event (yes, you need it for online game
//! with real time battle. Missed events should be ignored, so that the game
//! can continue), and the order of reception / emission do not count,
//! then you may want to look at a UDP library. SedNL provide reliability and a
//! high level API, and it has a cost (small, but not null). If you need to send
//! a huge amount of events (let's says hundread by second by client), then
//! maybe you'll prefer an UDP library. Otherwise, you'll probably prefer
//! the reliability and simple API provided by SedNL.
//!
//! \section example A short example
//!
//! A simple client :
//! \code
//! #include <SEDNL/sednl.hpp>
//!
//! using namespace SedNL;
//!
//! int main(int argc, char* argv[])
//! {
//!     TCPClient client(SocketAddress(4242, "localhost"));
//!
//!     client.send(Event("hello_msg", make_packet("Hello world")));
//!
//!     client.disconnect();
//!
//!     return EXIT_SUCCESS;
//! }
//! \endcode
//!
//! And a simple server :
//! \code
//! #include <SEDNL/sednl.hpp>
//!
//! using namespace SedNL;
//!
//! void on_disconnect(Connection&)
//! {
//!    std::cout << "Client disconnected" << std::endl;
//! }
//!
//! void on_connect(Connection&)
//! {
//!     std::cout << "Client connected" << std::endl;
//! }
//!
//! void on_event(Connection&, const Event&)
//! {
//!     std::cout << "Event from a client" << std::endl;
//! }
//!
//! int main(int argc, char* argv[])
//! {
//!     TCPServer server(SocketAddress(4242));
//!     EventListener listener(server);
//!     listener.on_connect().set_function(on_connect);
//!
//!     EventConsumer consumer(listener);
//!     consumer.on_disconnect().set_function(on_disconnect);
//!     consumer.on_event().set_function(on_event);
//!
//!     listener.run();
//!     consumer.run();
//!
//!     // Sleep
//!     while (true)
//!         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//!
//!    listener.join();
//!    consumer.join();
//!
//!     return EXIT_SUCCESS;
//! }
//! \endcode
//!
//! \section learn_more Learn more
//!
//! If you wan't to learn more about SedNL, I would recomand you
//! to read the documentation of classes EventListener, EventConsumer,
//! Packet and Event, a quick look at TCPClient / TCPServer, and then
//! the SEDNL_SERIALIZABLE macro. Once you read that, you'll prety well
//! understand the whole library.
//!

