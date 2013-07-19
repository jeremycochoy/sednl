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

#include "SEDNL/RingBuf.hpp"
#include "SEDNL/Event.hpp"
#include "SEDNL/Packet.hpp"

#include <iostream>

using namespace SedNL;

#define ASSERT(exp, msg) {if (!(exp)) { std::cerr << msg << std::endl; return EXIT_FAILURE; }}

int main()
{
    //Test case 1 : Filling the buffer from the begining
    {
        RingBuf buf(512);

        ASSERT(buf.size() == 512, "Buffer illsized");

        ASSERT(buf.length() == 0, "Empty buffer should be of length 0");

        for (int i = 0; i < 10; i++)
            ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 26) == true, "Can't put data");
        ASSERT(buf.length() == 260, "Filed buffer should be of length 260");

        //Fill to reach 500
        for (int i = 0; i < 24; i++)
            ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 10) == true, "Can't put data");

        ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 12) == true, "Can't reach 512");

        ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 1) == false, "Buffer overflow");

        ASSERT(buf.size() == 512, "Buffer size changed?!");
    }

    {
        RingBuf buf(30);

        ASSERT(buf.size() == 30, "Buffer illsized");

        ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 26) == true, "Can't put data");

        buf.reset();

        ASSERT(buf.length() == 0, "Buffer wasn't reseted");

        //Rely on the fact reset do m_start = m_end.

        ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 26) == true, "Error while making a turn");

        ASSERT(buf.length() == 26, "Buffersize should be 26");

        ASSERT(buf.put("abcdefghijklmnopqrstuvwxyz", 26) == false, "Buffer overflow");

    }

    //This time we check pick_event
    for (int i = 0; i < 3; i++)
        //Do it 3 times, because we want to make some turns inside buf
    {
        Event e;
        RingBuf buf(15);

        //8 bytes packet, abc name, then a char (1=Int8) with value 5.
        //Notice the bigendian (network order) for the header size.
        ASSERT(buf.put("\0\010abc\0\1\5", 8) == true, "Can't put data");

        ASSERT(buf.pick_event(e) == true, "Can't pick event!");
        ASSERT(e.get_name() == "abc", "Wrong name");

        Int8 c;
        Packet p = e.get_packet();
        //May throw a badtype exception
        try
        {
            p >> c;
        }
        catch(...)
        {
            ASSERT(false, "Reading data as Int8 failed");
        }
        ASSERT(c == 5, "Wrong value read");
    }

    //HUGE SUCCESS :)
    return EXIT_SUCCESS;
}
