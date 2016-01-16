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

#ifndef SERIALIZER_IPP_
#define SERIALIZER_IPP_

namespace SedNL
{

//! @cond

//! Implementation of the serialisation of
//! a used data type. See 'Serializer.hpp' for
//! use case.
template<class T>
struct Serializer
{
    template<class = decltype(&T::before_serialization)>
    static inline
    void pre_serialize(T& object, int)
    {
        object.before_serialization();
    };

    template<class U>
    static inline
    void pre_serialize(T&, U)
    {};

    template<class = decltype(&T::after_serialization)>
    static inline
    void post_serialize(T& object, int)
    {
        object.after_serialization();
    };

    template<class U>
    static inline
    void post_serialize(T&, U)
    {};

    template<class = decltype(&T::before_unserialization)>
    static inline
    void pre_unserialize(T& object, int)
    {
        object.before_unserialization();
    };

    template<class U>
    static inline
    void pre_unserialize(T&, U)
    {};

    template<class = decltype(&T::after_unserialization)>
    static inline
    void post_unserialize(T& object, int)
    {
        object.after_unserialization();
    };

    template<class U>
    static inline
    void post_unserialize(T&, U)
    {};
};

template<typename T, typename... Args>
void serializer_serialize(Packet& packet, T& inst, Args&... args)
{
    SedNL::Serializer<T>::pre_serialize(inst, 0);
    write_as_object(packet, args...);
    SedNL::Serializer<T>::post_serialize(inst, 0);
}

template<typename T, typename... Args>
void serializer_unserialize(PacketReader& packet_reader, T& inst, Args&... args)
{
    SedNL::Serializer<T>::pre_unserialize(inst, 0);
    read_as_object(packet_reader, args...);
    SedNL::Serializer<T>::post_unserialize(inst, 0);
}

} //namespace SedNL

//! @endcond

#endif /* !SERIALIZER_IPP_ */
