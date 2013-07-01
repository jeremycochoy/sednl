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

#ifndef EXEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>

#include "Export.hpp"

namespace SedNL
{

    //////////////////////////////////////////////
    //! \brief The root of SedNL's exception tree.
    //////////////////////////////////////////////
    class SEDNL_API Exception : virtual public std::exception
    {};

    ///////////////////////////////////////////
    //! \brief Different kind of TypeException.
    ///////////////////////////////////////////
    enum class SEDNL_API TypeExceptionT
    {
        Unknown,
        UserDataWrongTypeAsked,
        UserDataWrongTypeGiven,
    };

    //////////////////////////////////////////////
    //! \brief Different kind of NetowrkException.
    //////////////////////////////////////////////
    enum class SEDNL_API NetworkExceptionT
    {
        InvalidSocketAddress,
        CantRetrieveHost,
        ConnectFailed,
    };

    //////////////////////////////////////////////
    //! \brief Different kind of PacketException.
    //////////////////////////////////////////////
    enum class SEDNL_API PacketExceptionT
    {
    };

    ////////////////////////////////////////////////////
    //! \brief Describe how to build a SedNL::Exception.
    ////////////////////////////////////////////////////
    template<typename T>
    class SEDNL_API TemplateException : virtual public Exception
    {
    public:
        //! \brief Create an Exception
        inline TemplateException(T type);

        //! \brief Create an exception with a message
        //!
        //! The message should be a static string (you shouldn't
        //! use string from std::string::c_str() oranything that
        //! will be destructed / freed).
        inline TemplateException(T type, const char* msg);

        //! \brief Return the type of the exception
        inline T getType();

        //! \brief Return a message describing the exception,
        //!        based on it's type.
        virtual const char* what() const noexcept;
    private:
        T m_type;
        const char* m_msg;
    };


    ////////////////////////////////////////////////////
    //! \brief Exception throw when the type asked/given
    //!        isn't the type expected.
    ////////////////////////////////////////////////////
    typedef TemplateException<TypeExceptionT> TypeException;

    ///////////////////////////////////////////////////
    //! \brief Exception throw by the low level network
    //! part of the library.
    ///////////////////////////////////////////////////
    typedef TemplateException<NetworkExceptionT> NetworkException;

    ////////////////////////////////////////////////////
    //! \brief Exception throw when packet are read with
    //!        the wrong format.
    ////////////////////////////////////////////////////
    typedef TemplateException<PacketExceptionT> PacketException;

} // namespace SedNL

#include "Exception.ipp"

#endif /* !EXCEPTION_HPP_ */
