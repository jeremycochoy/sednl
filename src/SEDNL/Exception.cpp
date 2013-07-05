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

#include "SEDNL/Exception.hpp"

namespace SedNL
{

template<>
const char* TemplateException<NetworkExceptionT>::what() const noexcept
{
    switch(m_type)
    {
    case NetworkExceptionT::InvalidSocketAddress:
        return "The SocketAddress object given is invalid for it's purpose.";
    case NetworkExceptionT::CantRetrieveHost:
        return "Can't retrieve host ip from the host name.";
    case NetworkExceptionT::ConnectFailed:
        return "Can't connect to the server.";
    default:
        return "Unknown exception.";
    }
}

template<>
const char* TemplateException<TypeExceptionT>::what() const noexcept
{
    return "TODO";
}

template<>
const char* TemplateException<EventExceptionT>::what() const noexcept
{
    return "TODO";
}

}
