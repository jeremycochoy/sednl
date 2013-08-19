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

#ifndef THREAD_HELP_HPP
#define THREAD_HELP_HPP

#include <mutex>
#include <queue>

namespace SedNL
{

////////////////////////////////////////////////////////////
//! \brief Allow to protect a type T with a mutex.
////////////////////////////////////////////////////////////
template<class T>
class SafeType
{
public:
    //! \brief Create an atomic T from a value \a v.
    //!
    //! \param[in] v Initial value.
    inline SafeType(T v) noexcept;

    //! \brief Conversion from SafeType<T> to T.
    //!
    //! This operator use the internal mutex
    //! to allow using this object in a multithreaded
    //! environment.
    //!
    //! \return The value stored.
    inline operator T () const;

    //! \brief Change the value stored.
    //!
    //! This operator use the internal mutex
    //! to allow using this object in a multithreaded
    //! environment.
    //!
    //! \param[in] v Value to store.
    //! \return A reference to this.
    inline SafeType<T>& operator=(T v);
private:
    T m_value;
    mutable std::mutex m_mutex;
};

////////////////////////////////////////////////////////////
//! \brief A std::queue like protected with a mutex.
////////////////////////////////////////////////////////////
template<class T, class Container = std::deque<T>>
class SafeQueue
{
public:
    //! \brief Checks whether the underlying container is empty.
    //!
    //! \return True if empty, False otherwise.
    inline bool empty() const noexcept;

    //! \brief Return the size of the underlying container.
    //!
    //! \return Number of elements stored.
    inline typename Container::size_type size() const noexcept;

    //! \brief Push elements to the back of the queue.
    //!
    //! \param[in] value The value to push.
    //! \return True if it succeed, False if it failed.
    inline bool push(const T& value) noexcept;

    //! \brief Remove the first element and store it into \a value.
    //!
    //! If the queue is empty, return false and do not modify \a value.
    //!
    //! \param[in] value Where to store the data read.
    //! \return True if it succeed, False if it failed.
    inline bool pop(T& value) noexcept;

private:
    mutable std::mutex m_mutex;
    typedef Container QType;
    QType m_queue;
};

} // namespace SedNL

#include "ThreadHelp.ipp"

#endif /* !THREAD_HELP_HPP */

////////////////////////////////////////////////////////////
//!
//! \file ThreadHelp.hpp
//! \brief Some usefull classes to work with threads.
//!
////////////////////////////////////////////////////////////
