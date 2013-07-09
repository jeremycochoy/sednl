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

#ifndef THREAD_HELP_IPP
#define THREAD_HELP_IPP

#include <iostream>

namespace SedNL
{

template<class T>
SafeType<T>::SafeType(T v) noexcept
{
    m_value = v;
}

template<class T>
SafeType<T>::operator T()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_value;
}

template<class T>
SafeType<T>& SafeType<T>::operator=(T v)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_value = v;
    return *this;
}

template<class T, class C>
bool SafeQueue<T, C>::empty() const noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
            std::cerr << "Error: std::mutex::lock failed."
                      << std::endl;
            std::cerr << e.what() << std::endl;
#endif /* SEDNL_NOWARN */

    }
    return false;
}

template<class T, class C>
bool SafeQueue<T, C>::pop(T& value) noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return false;
        value = m_queue.front();
        m_queue.pop_front();
        return true;
    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
            std::cerr << "Error: std::mutex::lock failed."
                      << std::endl;
            std::cerr << e.what() << std::endl;
#endif /* SEDNL_NOWARN */

    }
    return false;
}

template<class T, class C>
bool SafeQueue<T, C>::push(const T& value) noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_back(value);
        return true;
    }
    catch(std::bad_alloc &e)
    {
#ifndef SEDNL_NOWARN
            std::cerr << "Arning: can't allocate while pushing in queue."
                      << std::endl;
            std::cerr << e.what() << std::endl;
#endif /* SEDNL_NOWARN */

    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
            std::cerr << "Error: std::mutex::lock failed."
                      << std::endl;
            std::cerr << e.what() << std::endl;
#endif /* SEDNL_NOWARN */

    }
    return false;
}

} // namespace SedNL

#include "ThreadHelp.ipp"

#endif /* !THREAD_HELP_IPP */
