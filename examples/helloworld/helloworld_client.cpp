#include <SEDNL/Connection.h>
#include <SEDNL/TCPClient.h>
#include <SEDNL/Event.h>

#include <iostream>
#include <cstdlib>

using namespace SedNL;

on_event(Connection &connection, const Event &e)
{
    //Event name
    std::cout << "Event from server :"
              << e.get_name()
              << std::endl;

    //Display the whole packet
    std::cout << e.get_packet() << std::endl;
}

int main(int argc, char *argv[])
{
    try
    {
        //Create a client and connect it to localhost, port 1337
        TCPClient client(SocketAddress(1337, "localhost"));

        //Set the on_event function
        client.on_event().set_function(on_event);

        //Create a packet
        Packet packet;
        //Put a random string
        packet << "Hello world";
        //Put a random number
        packet << 42;

        //Create an event and send it to the server
        client.send(Event("hello_msg", packet));

        //Wait 100ms
        Time::sleep(100);

        //Non blocking processing. It check if new event are
        //Available and call the on_something if needed.
        client.process();

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
