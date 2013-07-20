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
#include "SEDNL/Event.hpp"
#include "SEDNL/EventListener.hpp"
#include "SEDNL/SocketHelp.hpp"

#ifdef SEDNL_WINDOWS
#else /* SEDNL_WINDOWS */

#include <unistd.h>

#endif /* SEDNL_WINDOWS */

namespace SedNL
{

void Connection::disconnect() noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_listener)
            m_listener->tell_disconnected(this);
        unsafe_disconnect();
    }
    catch(std::exception &e)
    {
        warn_lock(e, "Connection::disconnect()");
        if (m_listener)
            m_listener->tell_disconnected(this);
        unsafe_disconnect();
    }
}

void Connection::safe_disconnect() noexcept
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        unsafe_disconnect();
    }
    catch(std::exception &e)
    {
        warn_lock(e, "Connection::safe_disconnect()");
        unsafe_disconnect();
    }
}

void Connection::unsafe_disconnect() noexcept
{
    if (m_connected)
    {
        close(m_fd);
        m_fd = -1;
        m_connected = false;
    }
}

void Connection::send(const Event& event)
{
    try
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        ByteArray data = event.pack();
        const char* data_ptr = reinterpret_cast<const char *>(&data[0]);
        //TODO : Handle write errors with exceptions
        write(m_fd, data_ptr, data.size());
    }
    catch(std::exception &e)
    {
#ifndef SEDNL_NOWARN
            std::cerr << "Error: "
                      << "std::mutex::lock failed in SedNL::Connection::send"
                      << std::endl;
        std::cerr << e.what() << std::endl;
#endif /* SEDNL_NOWARN */
    }
}

void Connection::set_user_data(const char* data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::String)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_string = data;
    m_data_type = UserDataType::String;
}

void Connection::set_user_data(int data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Int)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_int = data;
    m_data_type = UserDataType::Int;
}

void Connection::set_user_data(char data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Char)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_char = data;
    m_data_type = UserDataType::Char;
}

void Connection::set_user_data(float data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Float)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_float = data;
    m_data_type = UserDataType::Float;
}

void Connection::set_user_data(void* data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Ptr)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_ptr = data;
    m_data_type = UserDataType::Ptr;
}

void Connection::set_user_data(double data)
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::None
            && m_data_type != UserDataType::Double)
        throw TypeException(TypeExceptionT::UserDataWrongTypeGiven);

    m_data_double = data;
    m_data_type = UserDataType::Double;
}

void Connection::release_user_data() throw(std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_data_double = 0;
    m_data_type = UserDataType::None;
}

template<>
int Connection::get_user_data<int>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Int)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_int;
}

template<>
void* Connection::get_user_data<void*>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Ptr)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_ptr;
}

template<>
char Connection::get_user_data<char>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Char)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_char;
}

template<>
float Connection::get_user_data<float>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Float)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_float;
}

template<>
double Connection::get_user_data<double>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::Double)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_double;
}

template<>
const char* Connection::get_user_data<const char*>()
    throw(TypeException, std::system_error)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_data_type != UserDataType::String)
        throw TypeException(TypeExceptionT::UserDataWrongTypeAsked);

    return m_data_string;
}

}
