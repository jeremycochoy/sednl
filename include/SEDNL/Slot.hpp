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

#ifndef SLOT_HPP_
#define SLOT_HPP_

#include <unordered_map>
#include <memory>

namespace SedNL
{

template <typename... Arguments>
class Slot
{
public:
    //! \brief Type of a callback function stored by this slot
    typedef void (*Callback)(Arguments...);

    //! \brief Type of a member function (of a T object)
    //!        callback that can be stored by this slot.
    template <typename T>
    using MemberCallback = void (T::*)(Arguments...);

    //! \brief Remove an eventualy stored callback
    void reset() noexcept;

    //! \brief Call the current callback, or do nothing
    void operator()(Arguments... args);

    //! \brief Check if the slot is empty
    explicit operator bool() const noexcept;

    //! \brief Set the callback
    //!
    //! Set the callback from a function pointer, a lambda
    //! or a functor. Actualy, it just wrap T to the internal
    //! std::function, so you can also give a std::function,
    //! for example from std::bind.
    //!
    //! \argument callback The callback
    template<typename T>
    void set_function(T function);

    //! \brief Set the callback from an object
    //!
    //! Set a member function as a callback. The instance
    //! gien shouldn't die before the callback was removed
    //! with reset(), or the destructor ~Slot() called.
    //!
    //! \argument instance The instance that will be stored
    //! \argument callback The member function
    template<typename T>
    void set_function(T& instance, MemberCallback<T> callback);

    //! \brief Set the callback from a pointer to an object
    //!
    //! See set_function()
    template<typename T>
    void set_function(T* instance, MemberCallback<T> callback);

private:
    std::function<void(Arguments...)> container;
};

} // namespace SedNL

#include "SEDNL/Slot.ipp"

#endif /* !SLOT_HPP_ */
