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

//! \brief Interface of a callback container
template <typename... Arguments>
class CallbackContainer
{
public:
    //\brief Call the contained callback
    virtual void call(Arguments...);
};

template <typename... Arguments>
class SimpleFunctionCallback;

template <typename T, typename... Arguments>
class MemberFunctionCallback;

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
    void call(Arguments... args);

    //! \brief Set the callback
    //!
    //! \argument callback The callback
    void set_function(Callback callback);

    //! \brief Set the callback from an object
    //!
    //! Set a member function as a callback. The instance
    //! gien shouldn't die before the callback was removed
    //! with reset(), or the estructor ~Slot() called.
    //!
    //! \argument instance The instance that will be stored
    //! \argument callback The membr function
    template<typename T>
    void set_function(T instance, Callback callback);

    //! \brief Set the callback to be a functor or a lambda
    template<typename T>
    void set_function(T lambda);

private:
    std::shared_ptr< CallbackContainer<Arguments...> > container;
};

//! \brief A hash map to retrieve quickly slots
template <typename... Arguments>
using SlotMap = std::unordered_map< std::string, Slot<Arguments...> >;

//! \brief A container of a simple function
template <typename... Arguments>
class SimpleFunctionCallback : CallbackContainer<Arguments...>
{
public:
    //! \brief Store a callback in a new container
    inline
    SimpleFunctionCallback(typename Slot<Arguments...>::Callback callback);

    //! \brief Call the callback
    virtual void call(Arguments...);

private:
    //! The callback to call
    typename Slot<Arguments...>::Callback m_callback;
};

//! \brief A container for a lambda or a functor
template <typename T, typename... Arguments>
class LambdaCallback : CallbackContainer<Arguments...>
{
public:
    //! \brief Store a lambda in a new container
    inline
    LambdaCallback(T lambda);

    //! \brief Call the lambda/functor
    virtual void call(Arguments...);

private:
    //! The lambda/functor
    T m_lambda;
};

//! \brief A container of a member function
template <typename T, typename... Arguments>
class MemberFunctionCallback : CallbackContainer<Arguments...>
{
private:
    //! \brief Alias the type
    typedef
    typename Slot<T, Arguments...>::MemberCallback CallbackType;

public:
    //! \brief Store a member callback in a new container
    inline MemberFunctionCallback(T* instance, CallbackType callback);

    //! \brief Call the callback
    virtual void call(Arguments...);

private:
    //! \brief the instance
    T* m_instance;

    //! \brief The memeber callback to call
    CallbackType m_callback;
};

} // namespace SedNL

#include "Slot.ipp"

#endif /* !SLOT_HPP_ */
