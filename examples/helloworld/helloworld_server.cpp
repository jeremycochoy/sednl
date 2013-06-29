// Copyright (c) 2013 Jeremy S. Cochoy
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

