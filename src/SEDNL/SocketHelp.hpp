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

#ifndef SOCKET_HELP_HPP_
#define SOCKET_HELP_HPP_

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <unistd.h>
#include <fcntl.h>

#endif /* SEDNL_WINDOWS */

namespace SedNL
{

    //! \brief Set a socket file descriptor non blocking
    inline bool set_non_blocking(int fd)
    {
        int flags = 0;
        if ((flags = fcntl(fd, F_GETFL)) < 0)
            return false;
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0)
            return false;
        return true;
    }

} // namespace SedNL

#endif /* !SOCKET_HELP_HPP_ */
