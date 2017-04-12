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

#ifndef CONNECTION_BUFFER_SIZE
# define CONNECTION_BUFFER_SIZE 4096
#endif /* !MAX_CONNECTIONS */

#include "SEDNL/Export.hpp"
#include "SEDNL/Exception.hpp"
#include "SEDNL/SocketInterface.hpp"
#include "SEDNL/RingBuf.hpp"

#include <iostream>

namespace SedNL
{

    class Packet;
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
    //! \brief Construct an empty connection.
    inline Connection();

    //! \brief Close the connection if needed.
    //!
    //! The connection is closed without thread safety, and
    //! without sending any message to an eventual parent listener.
    inline ~Connection() noexcept;

    //! \brief Close the connection.
    //!
    //! This function is thread safe. You can use it
    //! to close a connection while processing events.
    //!
    //! For example :
    //! \code
    //! void on_pear(Connect& c, const Event&)
    //! {
    //!     c.disconnect();
    //! }
    //!
    //! //...
    //!
    //! EventConsumer consumer;
    //! consumer.bind("pear_event").set_function(on_pear);
    //! \endcode
    virtual void disconnect() noexcept;

    //! \brief Send the event \a event through the connection.
    //!
    //! Send an event packet. It can throw exceptions
    //! (std::exception or NetworkException) if failed.
    //!
    //! This function is thread safe, and you can use it to answer
    //! to events with events.
    //!
    //! For example :
    //! \code
    //! void on_pear(Connect& c, const Event&)
    //! {
    //!     c.send(make_event("answer", 42));
    //! }
    //!
    //! //...
    //!
    //! EventConsumer consumer;
    //! consumer.bind("pear_event").set_function(on_pear);
    //! \endcode
    //!
    //! \param[in] event The event to send.
    void send(const Event& event) throw(NetworkException, std::exception);

    //! \brief Create an event and send it through the connection.
    //!
    //! Same as:
    //! \code
    //! send(Event(name, packet));
    //! //...
    //!
    //! \param[in] name Name of the event.
    //! \param[in] packet The data attached.
    void send(std::string name, const Packet& packet) throw(NetworkException, std::exception);

    //! \brief Link a value to this connection.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store. If the string was allocated,
    //!                 you shouldn't free it before the connection was
    //!                 destroyed, or the resource released.
    void set_user_data(const char* data)
        throw(TypeException, std::system_error);

    //! \brief Link a value to this connection.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(int data)
        throw(TypeException, std::system_error);

    //! \brief Link a value to this connection.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(char data)
        throw(TypeException, std::system_error);

    //! \brief Link a value to this connection.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(float data)
        throw(TypeException, std::system_error);

    //! \brief Link a value to this connection.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \param[in] data Value to store.
    void set_user_data(double data)
        throw(TypeException, std::system_error);

    //! \brief Link a value to this connection.
    //!
    //! You can use this function to store a "User" instance containing
    //! all the data relate to this connection. If you do so, you'll
    //! have to be sure you are always using the right type (it's a void pointer),
    //! and take care of releasing/freeing the resource.
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
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
    //!   - int
    //!   - float
    //!   - double
    //!   - char
    //!   - void*
    //!   - const char*
    //!
    //! \return The value previously stored by set_user_data().
    template<class T>
    T get_user_data()
        throw(TypeException, std::system_error);

    //! \brief Release a resource stored via Connection::set_user_data().
    //!
    //! Warning: if you stored an object as a void* pointer and want
    //! to free the memory allocated (or delete an instance), you should
    //! retrieve the pointer before calling release_user_data().
    void release_user_data() throw(std::system_error);

private:
    //! \brief List of types that can be stored.
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

    //! \brief The type stored.
    UserDataType m_data_type;

    //! \brief Value stored.
    union
    {
        const char* m_data_string;
        int m_data_int;
        char m_data_char;
        float m_data_float;
        void* m_data_ptr;
        double m_data_double;
    };

    //! \brief The listener listening to this connection.
    EventListener *m_listener;

    void unsafe_disconnect() noexcept;
    void safe_disconnect() noexcept;

    //! \brief Network input stream (partialy received events
    //!        are stored here).
    RingBuf m_buffer;

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
//! 'send an event' throught a server. But, when you receive an event,
//! or when incoming connection are created, you callback are called
//! with a Connection object.
//!
//! std::system_error exceptions come from std::mutex::lock that can
//! fail. You can handle them or just think that if the system refuse
//! to lock a mutex, your application won't work anymore, and you don't
//! care.
//!
//! When a user connect to your server, you often want to keep
//! an instance associated to this user, which will be freed when
//! he will disconnect.
//!
//! This can be a index, a pointer, a binary string...
//!
//! SedNL provide many set_user_data() functions with different types.
//! Only __one__ value can be stored at the same type. Before setting a new
//! value of a different type, you have to release the resource by calling
//! release_user_data().
//! Then, the template function get_user_data() won't allow you to take the value
//! as a different type. If you put an integer, and then ask for a floating number,
//! it will throw an exception. This is done to allow dectection of bug in
//! early development stages.
//!
//! Here is an exemple on how you can handle users :
//! \code
//!
//! struct User
//! {
//!     User()
//!         :user_name("guest"), password(""), age(-1), logged(false)
//!     {};
//!
//!     std::string user_name;
//!     std::string password;
//!     int         age;
//!     bool        logged;
//! };
//!
//! void on_connect(Connection& c)
//! {
//!     User* user = new User;
//!     c->set_user_data((void*)user);
//! }
//!
//! void on_login(Connection& c, Event &e)
//! {
//!     User* const user = (User*)c.get_user_data<void*>();
//!     if (user->logged)
//!     {
//!         c.send(make_event("already_logged", user->user_name));
//!         return;
//!     }
//!
//!     e >> user->user_name >> user->password >> user->age;
//!     user->logged = true;
//!     c.send(make_event("logged"));
//! }
//!
//! //...
//!
//! EventListener listener;
//! listener.on_connect().set_function(on_connect);
//!
//! EventConsumer consumer(listener);
//! consumer.bind("loggin").set_function(on_login);
//!
//! \endcode
////////////////////////////////////////////////////////////
