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

#ifndef TYPES_HPP_
#define TYPES_HPP_

// cstdin is a C++11 header that define fixed size integers,
// if they are available.
// Cf http://en.cppreference.com/w/cpp/header/cstdint
#include <cstdint>

namespace SedNL
{
    //Fixed size int
    typedef int8_t  Int8;
    typedef int16_t Int16;
    typedef int32_t Int32;
    typedef int64_t Int64;

    //A Byte
    typedef Int8 Byte;
}

#endif /* !TYPES_HPP_ */
