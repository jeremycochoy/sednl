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

#include <SEDNL/Connection.h>
#include <SEDNL/TCPClient.h>
#include <SEDNL/Event.h>

#include <iostream>
#include <cstdlib>

using namespace SedNL;

void on_event(Connection &connection, const Event &e)
{
    //Event name
    std::cout << "Event from server :"
              << e.get_name()
              << std::endl;

    //Display the whole packet
    std::cout << e.get_packet() << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        //Create a client and connect it to localhost, port 1337
        TCPClient client(SocketAddress(1337, "localhost"));

        //Create a packet
        Packet packet;
        //Put a random string
        packet << "Hello world";
        //Put a random number
        packet << 42;

        //Create an event and send it to the server
        client.send(Event("hello_msg", packet));

        //Create an event listener
        EventListener listener(client);

        //Create an event consumer
        EventConsumer consumer(listener);
        consumer.on_event().set_function(on_event);

        //Wait 1000ms
        Time::sleep(1000);

        //Join the listener and consumer threads
        listener.join();
        consumer.join();

        //Close connection (it's facultative, since the destructor of
        //client call disconnect).
        client.disconnect();
    }
    catch (Exception &e)
    {
        std::cout << "An exception occured :" << e.what() << std::endl;
        return EXIT_FAILED;
    }

    return EXIT_SUCCESS;
}
