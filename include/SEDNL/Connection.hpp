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

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/SocketInterface.hpp"
#include "SEDNL/RingBuf.hpp"

#include <iostream>

namespace SedNL
{

    class Event;
    class Connection;
    template<typename... Arguments>
    class Slot;
    class EventListener;

///////////////////////////////////////////////////////////////
//! \brief Define the the Connection type wich
//!        handle a connection and allow sending Event objects.
///////////////////////////////////////////////////////////////
class SEDNL_API Connection : public SocketInterface
{
public:
    //! \brief Construct an empty connection
    inline Connection();

    //! \brief Call disconnect
    inline ~Connection() noexcept;

    //! \brief Cut the connection
    virtual void disconnect() noexcept;

    //! \brief Send an event
    //!
    //! \param event The event to send
    void send(const Event& event);

    //! \brief Store a value specific to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store. If the string was allocated,
    //!                 you shouldn't free it before the connection was
    //!                 destroyed, or released.
    void set_user_data(const char* data)
        throw(TypeException, std::system_error);

    //! \brief Store a value specific to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(int data)
        throw(TypeException, std::system_error);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(char data)
        throw(TypeException, std::system_error);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(float data)
        throw(TypeException, std::system_error);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(double data)
        throw(TypeException, std::system_error);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! You can use this function to store a "User" instance containing
    //! all the data relate to this connection. If you do so, you'll
    //! have to be sure you are always using the right type, and take care
    //! of releasing/freeing the resource.
    //!
    //! An other solution is to use a connection id, and then use this id
    //! to retrive your data. See the integer version of Connection::set_user_data.
    //!
    //! \param[in] data Value to store.
    void set_user_data(void* data)
        throw(TypeException, std::system_error);

    //! \brief Retrieve previously stored data.
    //!
    //! You should call with the right type, otherwise it will raise
    //! a TypeException.
    //!
    //! If no values was set, it will also raise a TypeException.
    //!
    //! Types allowed are :
    //! int, float, double, char, void*, const char*
    //!
    //! \return The value previously stored by set_user_data().
    template<class T>
    T get_user_data()
        throw(TypeException, std::system_error);

    //! \brief Release a data stored via Connection::set_user_data.
    //!
    //! Warning, if you stored an object as a void* pointer, you should
    //! retrieve the pointer before calling release_user_data().
    void release_user_data() throw(std::system_error);

private:
    //! \brief List of types that can be stored
    enum class UserDataType
    {
        String,
        Int,
        Char,
        Float,
        Ptr,
        Double,
        None,
    };

    //! \brief The type stored
    UserDataType m_data_type;
    //! \brief User data
    union
    {
        const char* m_data_string;
        int m_data_int;
        char m_data_char;
        float m_data_float;
        void* m_data_ptr;
        double m_data_double;
    };

    //! \brief the current listener
    EventListener *m_listener;

    void unsafe_disconnect() noexcept;
    void safe_disconnect() noexcept;

//    //! \brief Buffer where data are stored
//    RingBuf buffer;

    friend class EventListener;
};


} // namespace SedNL

#include "SEDNL/Connection.ipp"

#endif /* !CONNECTION_HPP_ */

////////////////////////////////////////////////////////////
//! \class SedNL::Connection
//!
//! A Connection object is obtained as a connection from a client
//! (created by an EventListener) or a TCPClient object.
//!
//! TCPServer aren't connection, because it's meaningless to
//! 'send an event' throught a server. You should select one (or more)
//! users, which are represented as connections.
//!
//! std::system_error exceptions come from std::mutex::lock that can
//! fail. You can handle them or just think that if the system refuse
//! to lock a mutex, your application won't work anymore.
//!
////////////////////////////////////////////////////////////
