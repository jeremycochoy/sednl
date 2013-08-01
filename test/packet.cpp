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

    //Test case 2 : Build a packet too long
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

    //Test case 3 : Check that >> . << = id \forall Packet::Type
    {
        Packet p;

        try
        {
            p << "Hello";
            p << (Int8)42;
            p << (Int16)2;
            p << (Int32)3;
            p << (Int64)4;
            p << (UInt8)42;
            p << (UInt16)2;
            p << (UInt32)3;
            p << (UInt64)4;
            p << 42.0f;
            p << 3.14;

            PacketReader r(p);

            std::string s;
            r >> s;
            ASSERT(s == "Hello", "The message string was corrupted");

            Int8 i8;
            r >> i8;
            ASSERT(i8 == 42, "The message Int8 was corruped");

            Int16 i16;
            r >> i16;
            ASSERT(i16 == 2, "The message Int16 was corruped");

            Int32 i32;
            r >> i32;
            ASSERT(i32 == 3, "The message Int32 was corruped");

            Int64 i64;
            r >> i64;
            ASSERT(i64 == 4, "The message Int64 was corruped");


            UInt8 ui8;
            r >> ui8;
            ASSERT(ui8 == 42, "The message UInt8 was corruped");

            UInt16 ui16;
            r >> ui16;
            ASSERT(ui16 == 2, "The message UInt16 was corruped");

            UInt32 ui32;
            r >> ui32;
            ASSERT(ui32 == 3, "The message UInt32 was corruped");

            UInt64 ui64;
            r >> ui64;
            ASSERT(ui64 == 4, "The message UInt64 was corruped");

            float f;
            r >> f;
            ASSERT(f == 42.0f, "The message float was corruped");

            double d;
            r >> d;
            ASSERT(d == 3.14, "The message double was corruped");
        }

        catch(std::exception& e)
        {
            ASSERT(false, "TC3: An exception occured : " << e.what());
        }
    }


    //Test case 4 : Check that >> . << = id \forall Packet::Type (non array)
    {
        Packet p;

        try
        {
            p << std::vector<char>({1, 2, 3, 4});

            PacketReader r(p);

            std::vector<char> s;
            r >> s;

            ASSERT(s.size() == 4, "Vector corrupted (bad length)");
            ASSERT(s[0] == 1 &&
                   s[1] == 2 &&
                   s[2] == 3 &&
                   s[3] == 4, "Vector corrupted : wrong data");

        }
        catch(PacketException& e)
        {
            std::cout << (int)e.get_type() << std::endl;
            ASSERT(false, "TC4 : An exception occured : " << e.what());
        }
    }

    //Test case 5 : Check that >> . << = id \forall Packet::Type (Array)
    {
        Packet p;

        try
        {
            std::vector<UInt8>   v1({1, 2});
            std::vector<Int8>    v2({1, 2});
            std::vector<UInt16>  v3({1, 2});
            std::vector<Int16>   v4({1, 2});
            std::vector<Int32>   v5({1, 2});
            std::vector<UInt32>  v6({1, 2});
            std::vector<Int32>   v7({1, 2});
            std::vector<UInt64>  v8({1, 2});
            std::vector<Int64>   v9({1, 2});
            std::vector<float>  v10({1.f, 2.f});
            std::vector<double> v11({1., 2.});

            p << v1 << v2 << v3 << v4 << v5 << v6 << v7 << v8 << v9 << v10 << v11;

            std::vector<UInt8>   u1;
            std::vector<Int8>    u2;
            std::vector<UInt16>  u3;
            std::vector<Int16>   u4;
            std::vector<Int32>   u5;
            std::vector<UInt32>  u6;
            std::vector<Int32>   u7;
            std::vector<UInt64>  u8;
            std::vector<Int64>   u9;
            std::vector<float>  u10;
            std::vector<double> u11;

            PacketReader r(p);

            r >> u1 >> u2 >> u3 >> u4 >> u5 >> u6 >> u7 >> u8 >> u9 >> u10 >> u11;

            ASSERT(v1 == u1, "UInt8 vector corrupted");
            ASSERT(v2 == u2, "Int8 vector corrupted");
            ASSERT(v3 == u3, "UInt16 vector corrupted");
            ASSERT(v4 == u4, "Int16 vector corrupted");
            ASSERT(v5 == u5, "Int32 vector corrupted");
            ASSERT(v6 == u6, "UInt32 vector corrupted")
            ASSERT(v7 == u7, "Int32 vector corrupted");
            ASSERT(v8 == u8, "UInt64 vector corrupted");
            ASSERT(v9 == u9, "Int64 vector corrupted");
            ASSERT(v10 == u10, "Float vector corrupted");
            ASSERT(v11 == u11, "Double vector corrupted");
        }
        catch(PacketException& e)
        {
            std::cout << (int)e.get_type() << std::endl;
            ASSERT(false, "An exception occured : " << e.what());
        }
    }

    //HUGE SUCCESS :)
    return EXIT_SUCCESS;
}
