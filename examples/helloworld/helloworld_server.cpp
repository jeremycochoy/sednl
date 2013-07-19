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

#include <SEDNL/Connection.hpp>
#include <SEDNL/TCPServer.hpp>
#include <SEDNL/Event.hpp>
#include <SEDNL/EventListener.hpp>
#include <SEDNL/SocketAddress.hpp>

#include <iostream>
#include <cstdlib>

#include <chrono>
#include <thread>

using namespace SedNL;

int client_id = 0;

void on_disconnect(Connection &c)
{
    //Display client
    std::cout << "Client " << c.get_user_data<int>()
              << " disconnected." << std::endl;
}

void on_connect(Connection &c)
{
    //Set the client id
    c.set_user_data(client_id++);

    //Display client
    std::cout << "New client " << c.get_user_data<int>() << std::endl;

    //Send a hello message
//    c.send(Event("hello_client", Packet("Hello, client!")));
}

void on_event(Connection &c, const Event &e)
{
    //Display the client id
    std::cout << "Event from " << c.get_user_data<int>() << " : " << std::endl;

    //Display the whole event
//    std::cout << e;
}

int main(int argc, char* argv[])
{
    try
    {
        //Create a server ready to listen on port 1337
        TCPServer server(SocketAddress(1337));

        //Create a event listener
        EventListener listener(server);

        //Create a event consumer
//        EventConsumer consumer(listener);
//        consumer.on_connect().set_function(on_connect);
//        consumer.on_disconnect().set_function(on_disconnect);
//        consumer.on_event().set_function(on_event);

        //Listen for events in a second thread
        listener.run();
        //Consume events in a third thread
//        consumer.run();

        //We are in the first (main) thread.
        //Since we have nothing to do, we wait 1000ms,
        //to let the two other works.
        while (true)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    catch (Exception &e)
    {
        std::cout << "An exception occured :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

