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

#include <memory>

namespace SedNL
{

////////////////////////////////////////////////////////////
//! \brief A slot which can contain a callback.
////////////////////////////////////////////////////////////
template <typename... Arguments>
class Slot
{
public:
    //! \brief Type of a callback function stored by this slot.
    typedef void (*Callback)(Arguments...);

    //! \brief Type of a member function (of a T object)
    //!        callback that can be stored by this slot.
    template <typename T>
    using MemberCallback = void (T::*)(Arguments...);

    //! \brief Remove an eventualy stored callback.
    void reset() noexcept;

    //! \brief Call the current callback, or do nothing.
    void operator()(Arguments... args);

    //! \brief Check if the slot isn't empty.
    //!
    //! \return True if the slot contain a callback, False otherwise.
    explicit operator bool() const noexcept;

    //! \brief Set the callback.
    //!
    //! Set the callback from a function pointer, a lambda
    //! or a functor. Actualy, it just wrap T to the internal
    //! std::function constructor, so you can also give a std::function,
    //! for example from a call to std::bind.
    //!
    //! \param[in] function The callback to store.
    template<typename T>
    void set_function(T function);

    //! \brief Set the callback from an object.
    //!
    //! Set a member function as a callback. The instance
    //! given shouldn't die before the callback was removed
    //! with reset(), or the destructor ~Slot() called.
    //! Otherwise, it will result in undefined behavior.
    //!
    //! Warning : This set_function() version store a copy of the instance.
    //! If you want to give a reference, see
    //! set_function(T*, MemberCallback<T>).
    //!
    //! \param[in] instance The instance that will be copied.
    //! \param[in] callback The member function.
    template<typename T>
    void set_function(T& instance, MemberCallback<T> callback);

    //! \brief Set the callback from a pointer to an object.
    //!
    //! This set_function() version store the pointer of the instance,
    //! and a pointer to member function.
    //!
    //! Same use as set_function(T&, MemberCallback<T>) except that
    //! the instance given isn't copied. It's the most common use case.
    //!
    //! \param[in] instance The instance pointer that will be stored.
    //! \param[in] callback The member function.
    template<typename T>
    void set_function(T* instance, MemberCallback<T> callback);

private:
    std::function<void(Arguments...)> container;
};

} // namespace SedNL

#include "SEDNL/Slot.ipp"

#endif /* !SLOT_HPP_ */

////////////////////////////////////////////////////////////
//!
//! \class SedNL::Slot
//!
//! A slot is an emplacement that can store callback.
//!
//! A callback can be :
//!   - A function pointer, as `void (*function)(int)`.
//!   - A functor, wich means a structure providing an implementation
//!     of `operator()`.
//!   - A lambda, for example `[] (int v) { std::cout << v; }`.
//!   - A std::function object (see std::function and std::bind
//!     documentation).
//!   - A pointer to member function, with a copy of an instance of
//!     the class.
//!   - A pointer to member function, with a reference to an instance of
//!     the class.
//!
//! Slot are the way you bind you callbacks to event consumers.
//!
//! \code
//!
//! void on_connect(Connection &c)
//! {
//!     std::cout << "New connection" << std::endl;
//! }
//!
//! class MyClass
//! {
//! public:
//!     MyClass()
//!         :v(42)
//!     {};
//!
//!     void on_hello(Connection& c, const Event& e)
//!     {
//!         c.send("hello_back", make_event("hello_back", 42));
//!     };
//! };
//!
//! // ...
//!
//! MyClass my_class;
//!
//! EventListener listener;
//! //listener.on_connect() is a reference to a Slot<Connection&>.
//! listener.on_connect().set_function(on_connect);
//!
//! EventConsumer consumer;
//! //consumer.bind("hello") is a reference to a Slot<Connection&, const Event&>.
//! consumer.bind("hello").set_function(&my_class, MyClass::on_hello);
//!
//! \endcode
////////////////////////////////////////////////////////////
