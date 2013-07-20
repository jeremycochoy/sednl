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

// Test cases, to check that ring buffer behave correctly

#include "SEDNL/EventListener.hpp"
#include "SEDNL/EventConsumer.hpp"

#include <iostream>

using namespace SedNL;

#define ASSERT(exp, msg) {if (!(exp)) { std::cerr << msg << std::endl; return EXIT_FAILURE; }}

int main()
{
    //Test case 1 : Shouldn't throw
    {
        try
        {
            EventListener listener;
            EventConsumer consumer(listener);

            consumer.on_connect().set_function([](Connection&){});
            consumer.on_disconnect().set_function([](Connection&){});
            consumer.on_server_disconnect().set_function([](TCPServer&){});
            consumer.on_event().set_function([](Connection&, const Event&){});

            consumer.bind("hellomsg").set_function([](Connection&, const Event&){});

            listener.run();
            listener.join();
        }
        catch(Exception& e)
        {
            ASSERT(false, "Test case 1 failed: \n" << e.what());
        }
    }

#define TEST_THROW(n, slot, lambda)                             \
    {                                                           \
        try                                                     \
        {                                                       \
            EventListener listener;                             \
            EventConsumer consumer1(listener);                  \
            EventConsumer consumer2(listener);                  \
                                                                \
            consumer1.slot.set_function(lambda);              \
            consumer2.slot.set_function(lambda);              \
                                                                \
            listener.run();                                     \
            listener.join();                                    \
                                                                \
            ASSERT(false, "Test case " n " failed: "            \
           "collision not detected on on_connect");             \
        }                                                       \
        catch(Exception& e)                                     \
        {}                                                      \
    }

    //Test cases 2-6 : Should throw
    TEST_THROW("2", on_connect(), [](Connection&){});
    TEST_THROW("3", on_disconnect(), [](Connection&){});
    TEST_THROW("4", on_server_disconnect(), [](TCPServer&){});
    TEST_THROW("5", on_event(), [](Connection&, const Event&){});
    TEST_THROW("6", bind("hellomsg"), [](Connection&, const Event&){});

    return EXIT_SUCCESS;
}
