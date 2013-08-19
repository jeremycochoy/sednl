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

#ifndef VERSION_HPP_
#define VERSION_HPP_

#include "SEDNL/Export.hpp"

namespace SedNL
{

namespace Version
{
    //! \brief Major library version.
    //!
    //! Increased in case of major changes / API break.
    extern const int SEDNL_API Major;
    //! \brief Minor library version.
    //!
    //! Increased for each new release.
    extern const int SEDNL_API Minor;
    //! \brief Micro library version.
    //!
    //! Increased in case of small fixes / bug fixes.
    extern const int SEDNL_API Micro;
}

} // namespace SedNL

#endif /* !VERSION_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \file Version.hpp
//!
//! Define version constants (Major, Minor et Micro) that are
//! read from the library.
//! It's usefull if your application is dinamically linked
//! to a library.
//!
////////////////////////////////////////////////////////////
