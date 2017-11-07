SedNL
=====

Simple Event Driven Network Library

__Last version : 1.0__

A network library
-----------------

SedNL is a C++11 network library with a high level API. It is event driven,
which means you bind functions on events and your callback will be asynchronously
called in a multi-threaded environment.
This reduce network to 'designing good events' and 'reacting to events'.

Events are mostly (but not only) typed messages. Typing (provided for free by the nice API)
allow to ensure the servers and clients speak the same protocol, and add more flexibility
in the structure of messages.

SedNL support asynchronous messaging through the use of TCP.

Building a hello world
----------------------

To check your installation and get started, you can compile the following code:
```c++
#include <SEDNL/sednl.hpp>
using namespace SedNL;
int main(int argc, char* argv[])
{
    TCPClient client(SocketAddress(4242, "localhost"));
    client.send(Event("hello_msg", make_packet("Hello world")));
    client.disconnect();
    return EXIT_SUCCESS;
}
```

with the compile line `g++ -std=c++11 -lpthread -lsednl file.cpp`.

Binding on events
-----------------

Binding on events is really easy. Here is a small sample :
```c++
// Your callbacks :
void my_on_connect(Connection& c);
void my_on_disconnect(Connection& c);
void my_hello_msg(Connection& c, const Event& e);
void my_say_something(Connection& c, const Event& e);
void my_apples(Connection& c, const Event& e);


// Create a server
TCPServer server(SocketAddress(7777));

// Bind my_on_connect and listen from server
EventListener listener(server);
listener.on_connect().set_function(my_on_connect);

// Bind some events
EventConsumer consumer(listener);
consumer.on_disconnect(my_on_disconnect);
consumer.bind("hello_msg").set_function(my_hello_msg);
consumer.bind("say_something").set_function(my_say_something);
consumer.bind("apples").set_function(my_apples);

// Launch listener thread and consumer thread
listener.run();
consumer.run();

// Do what you want in the main thread ...

// Stop listener, close connections, and wait for the end of event processing.
listener.join();
consumer.join();

```

Binary release (Version 1.0)
--------------

### Windows ###
Available soon.

### Linux ###
32 Bits beta release is [available here](http://zenol.fr/sednl/sednl_clang_m32.tar.gz).
64 Bits beta release is [available here](http://zenol.fr/sednl/sednl_clang_m64.tar.gz).

Compilation and Installation
----------------------------

### Linux ###

Some distributions provide a package. For example, on archlinux, you can type `yaourt -S sednl`.

**Installation from sources:** First, you have to `git clone https://github.com/Zenol/sednl.git`.
Then, create a build directory. We will use a _build_ directory in the clone : `cd sednl ; mkdir build ; cd build`.
And now, we generate all the makefiles with `cmake ..`.
Then, you are home, and you can type `make ; sudo make install`.

If you want to build the documentation, type `make doc`.

### Windows ###

You should use cmake.exe / cmake-gui.exe.
If you want to build in _sednl/build_, to generate MinGW makefiles you
can type `cmake -G "MinGW Makefiles" ..`.

Notice that Visual Studio's compiler isn't C++11 compliant, and it's probably that SedNL won't build.
Seams like VC++ 2013 will solve it. You can use Visual Studio with Clang++ or MinGW (or any C++11 compiler).

API documentation
-----------------

You can have a look to the _examples_ folder to get an idea of the flexibility of this API.

The full documentation (very detailed) is available here : http://zenol.fr/sednl/

Design
------

SedNL use a queue to store the received events. This is done by the `EventListener` class. Then, queues are
read by consumers thread. Creating a thread and setting callbacks is handled with the `EventConsumer` class.

For easy building of message, we provide a serialisation macro (although you can also write it by yourself) and rely on variadic templates. Events are instances of the `Event` class, which is nothing
more than a `Packet` instance bundled with a name (`std::string`). The packet class provide way to stack data and build an Event.

Short event packets can easily be written by hand. For complex object, you can serialise instances of your classes, so that your message reduce to few serialized object. By doing so, you get a more flexible and reusable design.

You may notice that half of the library code isn't networking, but providing a hight level API so that you don't
have to worry about handling concurency by hand, managing thread, protecting resources with mutexes, etc.

[![Build Status](https://drone.io/github.com/Zenol/sednl/status.png)](https://drone.io/github.com/Zenol/sednl/latest)
