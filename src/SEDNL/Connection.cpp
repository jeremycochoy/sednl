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

#include "SEDNL/Connection.hpp"

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <unistd.h>

#endif /* SEDNL_WINDOWS */

namespace SedNL
{

void Connection::disconnect() noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);

    //The call to close may fail, for example if the last write failed.
    //But what can we do to that? So we silently ignore the return value.
    if (m_connected)
        close(m_fd);
    m_connected = false;
}

void Connection::send(const Event& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    //TODO
    write(m_fd, "TODO", 4);
}

void Connection::set_user_data(const char* data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::String)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_string = data;
    m_data_type = UserDataType::String;
}

void Connection::set_user_data(int data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Int)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_int = data;
    m_data_type = UserDataType::Int;
}

void Connection::set_user_data(char data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Char)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_char = data;
    m_data_type = UserDataType::Char;
}

void Connection::set_user_data(float data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Float)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_float = data;
    m_data_type = UserDataType::Float;
}

void Connection::set_user_data(void* data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Ptr)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_ptr = data;
    m_data_type = UserDataType::Ptr;
}

void Connection::set_user_data(double data) throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Double)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_double = data;
    m_data_type = UserDataType::Double;
}

void Connection::release_user_data()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_data_double = 0;
    m_data_type = UserDataType::None;
}

template<>
int Connection::get_user_data<int>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Int)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_int;
}

template<>
void* Connection::get_user_data<void*>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Ptr)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_ptr;
}

template<>
char Connection::get_user_data<char>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Char)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_char;
}

template<>
float Connection::get_user_data<float>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Float)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_float;
}

template<>
double Connection::get_user_data<double>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Double)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_double;
}

template<>
const char* Connection::get_user_data<const char*>() throw(TypeException)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::String)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_string;
}

}
