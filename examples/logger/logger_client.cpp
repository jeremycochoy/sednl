#include <SEDNL/sednl.hpp>
#include <iostream>

//
// This application send a log to a server.
//
// The looger client expect a text input from stdin
// and send it to the server, tagger with the string
// name given as first argument.
//

using namespace SedNL;

// Read input from stdin
std::string get_input()
{
    std::string line;
    std::string output;

    while (std::getline(std::cin, line))
        output += line;

    return output;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << argv[0] << " listing_name" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        //Connect to server port 4280
        TCPClient client(SocketAddress(4280, "localhost"));

        //Read the input
        std::string input = get_input();

        //Send the code to log
        client.send(make_event("log_that", argv[1], input));

        client.disconnect();
    }
    catch(...)
    {
        std::cout << "Failed to connect" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
