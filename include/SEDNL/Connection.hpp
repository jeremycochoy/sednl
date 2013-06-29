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

#include "Export.hpp"
#include "Exception.hpp"

namespace SedNL
{

    class Event;
    class Connection;
    template<typename... Arguments>
    class Slot;

//-------------------------------------------------------------
//! \brief Define the the Connection type wich
//!        handle a connection and allow sending Event objects.
//-------------------------------------------------------------
class SEDNL_API Connection
{
public:
    //! \brief Construct an empty connection
    inline Connection();

    //! \brief Send an event
    //!
    //! \param event The event to send
    void send(const Event& event);

    //! \brief
    Slot<Connection&> on_disconnect();

    //! \brief
    Slot<Connection&, const Event&> on_event();

    void process();

    //! \brief Store a value specific to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \argument data Value to store. If the string was allocated,
    //!                you shouldn't free it before the connection was
    //!                destroyed, or released.
    void set_user_data(const char* data);

    //! \brief Store a value specific to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \argument data Value to store.
    void set_user_data(int data);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \argument data Value to store.
    void set_user_data(char data);

    //! \brief Store a value specifit to this connection
    //!
    //! This class assume that you will allways use the same
    //! datatype, until you released it with Connection::release_user_data().
    //!
    //! \argument data Value to store.
    void set_user_data(float data);

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
    //! \argument data Value to store.
    void set_user_data(void* data);

    //! \brief Retrieve previously stored data.
    //!
    //! You should call with the right type, otherwise it will raise
    //! a TypeException.
    //!
    //! If no values was set, it will also raise a TypeException.
    //!
    //! \return The value previously stored by set_user_data().
    template<class T>
    T get_user_data() throw(TypeException);

    //! \brief Release a data stored via Connection::set_user_data.
    //!
    //! Warning, if you stored an object as a void* pointer, you should
    //! retrieve the pointer before calling release_user_data().
    void release_user_data();
};


} // namespace SedNL

#endif /* !CONNECTION_HPP_ */
