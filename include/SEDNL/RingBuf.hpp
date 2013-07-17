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

#ifndef RING_BUF_HPP_
#define RING_BUF_HPP_

#include "SEDNL/Types.hpp"

#include <memory>

namespace SedNL
{

class Event;

//! \brief A ring buffer, for implementation purpose
//!
//! You probably won't have to use it, although you are modifying SEDNL's code.
//!
class RingBuf
{
public:

    //! \brief An iterator
    class Iterator
    {
    public:
        Iterator(RingBuf &buf);
        UInt8 operator* ();
        Iterator& operator++ ();
    private:
        unsigned int pos;
    };

    //! \brief Build a ring buffer of fixed size \a size
    RingBuf(unsigned int size) throw (std::bad_alloc);

    //! \brief Try tu put \a length characters into the buffer.
    //!
    //! \return False if failed (unmodified buffer), True otherwise.
    bool put(const char* string, unsigned int length) noexcept;

    //! \brief Return the buffer size
    //!
    //! \return size available
    inline
    unsigned int size() const noexcept;

    //! \brief Calculate the length user
    //!
    //! \return length used
    inline
    unsigned int length() const noexcept;

    //! \brief Reset the buffer to length 0
   inline
   void reset() noexcept;

    //! \brief Try to read a full event from the buffer.
    //!
    //! Build an event objet from the data. Erase the data from the buffer, and
    //! return the new event throught \a event.
    //! If it fails, it do not modify event.
    //!
    //! \return True if new event stored in \a event, False otherwise.
    bool pick_event(Event& event) noexcept;

private:
    std::unique_ptr<UInt8[]> m_dt;
    unsigned int m_size;
    unsigned int m_start;
    unsigned int m_end;

    friend Iterator;
};

} // namespace SedNL

#include "RingBuf.ipp"

#endif /* !RING_BUG_HPP_ */
