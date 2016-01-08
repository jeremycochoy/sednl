#include <SEDNL/sednl.hpp>
#include <iostream>
#include <mutex>

//
// This application is a logger server.
//
// The server wait for logs from the client,
// and display them in the standard output.
//

using namespace SedNL;

//Locked when writing to stdout
std::mutex log_mutex;

void on_log_that(Connection&, const Event& event)
{
    std::string log_name;
    std::string log_content;

    PacketReader reader = event.get_packet();
    reader >> log_name >> log_content;

    log_mutex.lock();
    std::cout << "--------------------" << std::endl;
    std::cout << ": " << log_name << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << log_content << std::endl;
    log_mutex.unlock();
}

int main(int /* argc */, char* /* argv */[])
{
    try
    {
        TCPServer server(SocketAddress(4280));

        EventListener listener(server);

        EventConsumer consumer(listener);
        consumer.bind("log_that").set_function(on_log_that);

        listener.run();
        consumer.run();

        // Sleep
        while (true)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        listener.join();
    }
    catch(...)
    {
        std::cout << "Failed to connect" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
