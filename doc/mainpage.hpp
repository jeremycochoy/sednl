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
//! \section example A short example
//!
//! Here is a simple example to show how you can set up client and server
//! in few lines.
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
//! \section description Description
//! SedNL (for Simple Event Driven Network Library) is a C++11 library
//! that provide a simple hight level API to send and receive easily
//! events through network.
//!
//! An event is a string (the event name) followed by some data.
//! You build your own events easily, and can serialize instances.
//! You easily bind callback (lambda, functor, function or member function)
//! to events, and don't have to worry on how to queue events
//! and handle concurency.
//!
//! This library is intended for application that want to communicate using
//! messages. Messages can't be lost, and will always be received in the same order as they
//! was send (althought they will be processed asynchronously).
//! This library is especialy suitable fo online video games.
//! For this purpose, the library rely on TCP.
//!
//! \section tcp_udp TCP or UDP ?
//!
//! The underlying protocol used by SedNL is TCP.
//!
//! \section learn_more Learn more
//!
//! If you wan't to learn more about SedNL, I would recomand you
//! to read the documentation of classes SedNL::EventListener, SedNL::EventConsumer,
//! Packet and Event, a quick look at SedNL::TCPClient / SedNL::TCPServer, and then
//! the \ref SEDNL_SERIALIZABLE macro. Once you read that, you'll prety well
//! understand the whole library.
//!
