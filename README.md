SedNL
=====

Simple Event Driven Network Library

__Version 1.0 is now in Beta!__

A network library
-----------------

SedNL is a C++11 network library with a high level API. It is event driven,
which means you bind functions on events and your callback will be asynchronously
called in a multi-threaded environment.
This reduce network to 'designing good events' and 'reacting to events'.

SedNL use TCP to send and receive events.

Binary release (Version 1.0)
--------------

### Linux ###
32 Bits beta release is [available here](http://zenol.fr/sednl/sednl_clang_m32.tar.gz).
64 Bits beta release is [available here](http://zenol.fr/sednl/sednl_clang_m64.tar.gz).

Installation
------------

### Linux ###

Installation is simple. First, you have to `git clone https://github.com/Zenol/sednl.git`.
Then, create a build directory. We will use a _build_ directory in the clone : `cd sednl ; mkdir build`.
And now, we generate all the makefiles with `cmake ..`.
Then, you are home, and you can type `make ; sudo make install`.

### Windows ###

You should use cmake.exe / cmake-gui.exe.
If you want to build in _sednl/build_, to generate MinGW makefiles you
can type `cmake -G "MinGW Makefiles" ..`.

Notice that Visual Studio's compiler isn't C++11 compliant, and it's probably that SedNL won't build.
Seams like VC++ 2013 will solve it. You can use Visual Studio with Clang++ or MinGW (or any C++11 compiler).


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

API documentation
-----------------

You can look at examples to have simple examples of use.

Documentation (very detailed) is available here : http://zenol.fr/sednl/


[![Build Status](https://drone.io/github.com/Zenol/sednl/status.png)](https://drone.io/github.com/Zenol/sednl/latest)
