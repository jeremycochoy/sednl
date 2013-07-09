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

//! \brief Allow to protect a type T with a mutex
template<class T>
class SafeType
{
public:
    inline SafeType(T v) noexcept;
    inline operator T ();
    inline SafeType<T>& operator=(T v);
private:
    T m_value;
    std::mutex m_mutex;
};

//! \brief A std::queue protected with a mutex
template<class T, class Container = std::deque<T>>
class SafeQueue
{
public:
    //! \brief Checks whether the underlying container is empty
    inline bool empty() const noexcept;

    //! \brief Inserts element at the end
    inline bool push(const T& value) noexcept;

    //! \brief Remove the first element and store it into \a value
    //!
    //! If the queue is empty, return false and do not modify \a value.
    //!
    //! \param[in] value Where to store the data
    inline bool pop(T& value) noexcept;

private:
    std::mutex m_mutex;
    typedef Container QType;
    QType m_queue;
};

} // namespace SedNL

#include "ThreadHelp.ipp"

#endif /* !THREAD_HELP_HPP */
