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

#include <SEDNL/Packet.hpp>
#include <SEDNL/Serializer.hpp>

#include <iostream>

using namespace SedNL;

class Foo
{
public:
    Foo()
        :m_int(0), m_char(0)
    {
        std::cout << "Foo constructed" << std::endl;
    };

    void set_things(char c, int n, std::string s)
    {
        m_char = c;
        m_int = n;
        m_string = s;
    }

    void show()
    {
        std::cout << "Foo instance : m_char = "
                  << m_char
                  << " m_int = "
                  << m_int
                  << " m_string = "
                  << m_string
                  << std::endl;
    }

    //
    // None of these methods are needed.
    //
    // If they are available they will be called.
    // Otherwise, your object will be serialised
    // without trying to call this methods.
    //
    // You can use them, to release or re-create data
    // that can't be transmited. For example, connection
    // to a database. (You may need serialized data before
    // connecting to database,
    // or you may want to release a connection before
    // unserialization. )

    void before_serialization()
    {
        std::cout << "Before serialization" << std::endl;
    };

    void after_serialization()
    {
        std::cout << "After serialization" << std::endl;
    };

    void before_unserialization()
    {
        std::cout << "Before unserialization" << std::endl;
    };

    void after_unserialization()
    {
        std::cout << "After unserialization" << std::endl;
    };

private:
    int m_int;
    char m_char;
    std::string m_string;

public:
    //
    // Allow serialisation
    //
    SEDNL_SERIALIZABLE(m_int, m_char, m_string);
};

int main(int, char*[])
{
    //Create and setup a Foo instance
    Foo first;
    first.set_things('*', 23, "Hello World");
    first.show();

    //Serialize the Foo instance
    Packet p;
    first.serialize(p);

    // Here, you would probably send the packet through the network

    //Create an other Foo instance
    Foo second;

    //Unserialize into the new instance
    PacketReader r(p);
    second.unserialize(r);

    //Display the content
    second.show();

    return EXIT_SUCCESS;
}
