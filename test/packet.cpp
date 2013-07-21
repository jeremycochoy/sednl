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

// Test cases, to check that packet integrity is well checked

#include "SEDNL/Packet.hpp"
#include "SEDNL/Event.hpp"
#include "SEDNL/RingBuf.hpp"

#include <iostream>

using namespace SedNL;

#define ASSERT(exp, msg) {if (!(exp)) { std::cerr << msg << std::endl; return EXIT_FAILURE; }}

int main()
{
    //Test case 1 : Build a good packet
    {
        Packet p;

        p << "Hello";
        p << (Int8)1;
        p << (Int16)2;
        p << (Int32)3;
        p << (Int64)4;
        p << (UInt8)1;
        p << "Hello World";
        p << (UInt16)2;
        p << (UInt32)3;
        p << (UInt64)4;
        p << 42.0f;
        p << 3.14;

        ASSERT(p.is_valid(), "Valid packet with is_valid() == false!");
    }

    //Test case 1 : Build a packet too long
    {
        RingBuf b(300);
        Event e;

        try
        {
            b.put("\0\010a\0\1aaa", 8);
            b.pick_event(e);
            //Should produce an warning output.
        }
        catch(...)
        {}
    }

    //HUGE SUCCESS :)
    return EXIT_SUCCESS;
}
