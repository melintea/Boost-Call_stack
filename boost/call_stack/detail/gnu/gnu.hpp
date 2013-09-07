/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */



#if !defined(BOOST_CALL_STACK_GNU_HPP)
#define BOOST_CALL_STACK_GNU_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#if defined(BOOST_CALL_STACK_NO_OPTIONAL_LIBS)
#  define BOOST_CALL_STACK_GNU_NO_LIBBFD
#endif

#include <boost/call_stack/detail/gnu/symbol.hpp>
#include <boost/call_stack/detail/gnu/frame.hpp>
#include <boost/call_stack/detail/gnu/stack.hpp>

#include <boost/static_assert.hpp>
#include <boost/assert.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {


/**
 * @return true if successful.
 */
bool init()
{
#if !defined(BOOST_CALL_STACK_GNU_NO_LIBBFD)
    bool ret = bfd::bfd_lib_type::instance().init();
    return ret;
#else
    return true;
#endif
}

/**
 * @return true if successful.
 */
bool shutdown()
{
#if !defined(BOOST_CALL_STACK_GNU_NO_LIBBFD)
    bool ret = bfd::bfd_lib_type::instance().shutdown();
    return ret;
#else
    return true;
#endif
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_GNU_HPP)
