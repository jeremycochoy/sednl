#include <SEDNL/Connection.h>
#include <SEDNL/TCPServer.h>
#include <SEDNL/Event.h>
#include <SEDNL/EventListener.h>

#include <iostream>
#include <cstdlib>

using namespace SedNL;

int client_id = 0;

void on_disconnect(Connection &e)
{
    //Display client
    std::cout << "Client " << e.get_data<int>()
              << " disconnected." << std::endl;
}

void on_connect(Connection &e)
{
    //Set the client id
    e.set_data(client_id++);

    //Display client
    std::cout << "New client " << e.get_data<int>() << std::endl;

    //Send a hello message
    e.send(Event("hello_client", Packet("Hello, client!")));
}

void on_event(Connection &e, const Event &e)
{
    //Display the client id
    std::cout << "Event from " << e.get_data<int>() << " : " << std::endl;

    //Display the whole event
    std::cout << e;
}

int main(int argc, char *argv[])
{
    try
    {
        //Create a server
        TCPServer server;

        //Set the on_connect function
        server.on_connect().setFunction(on_connect);

        //Listen on port 1337
        server.open(1337);

        //Create a listener
        EventListener listener(server);
        listener.on_connect().set_function(on_connect);
        listener.on_connect().set_function(on_disconnect);
        listener.on_connect().set_function(on_event);

        //Listen for events and dispatche them
        listener.listen();
    }
    catch (Exception &e)
    {
        std::cout << "An exception occured :" << e.what() << std::endl;
        return EXIT_FAILED;
    }
    return EXIT_SUCCESS;
}

