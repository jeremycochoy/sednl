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

#ifndef SLOT_IPP_
#define SLOT_IPP_

namespace SedNL
{

template <typename... Arguments>
void Slot<Arguments...>::call(Arguments... args)
{
    if (container)
        container->call(args...);
}

template <typename... Arguments>
void Slot<Arguments...>::reset() noexcept
{
    container.swap(std::shared_ptr< CallbackContainer<Arguments...> >(nullptr));
}

template <typename... Arguments>
SimpleFunctionCallback<Arguments...>::SimpleFunctionCallback(typename Slot<Arguments...>::Callback callback)
    :m_callback(callback)
{}


template <typename... Arguments>
void SimpleFunctionCallback<Arguments...>::call(Arguments... args)
{
    m_callback(args...);
}

template <typename T, typename... Arguments>
MemberFunctionCallback<T, Arguments...>
::MemberFunctionCallback(T* instance,
                         typename MemberFunctionCallback<T, Arguments...>
                         ::CallbackType callback)
    :m_instance(instance), m_callback(callback)
{}

template <typename T, typename... Arguments>
void MemberFunctionCallback<T, Arguments...>::call(Arguments... args)
{
    m_instance->*m_callback(args...);
}

template <typename T, typename... Arguments>
void LambdaCallback<T, Arguments...>::call(Arguments... args)
{
    m_lambda(args...);
}

template <typename T, typename... Arguments>
LambdaCallback<T, Arguments...>
::LambdaCallback(T callback)
    :m_lambda(callback)
{}

} // namespace SedNL

#endif /* !SLOT_IPP_ */
