/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_WIN_HPP)
#define BOOST_CALL_STACK_WIN_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif


#include <boost/call_stack/detail/win/symbol.hpp>
#include <boost/call_stack/detail/win/frame.hpp>
#include <boost/call_stack/detail/win/stack.hpp>

#include <boost/static_assert.hpp>
#include <boost/assert.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {


bool init()
{
    bool ret = dbghelp::dbghelp_lib_type::instance().init();
    return ret;
}

bool shutdown()
{
    bool ret = dbghelp::dbghelp_lib_type::instance().shutdown();
    return ret;
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_WIN_HPP)
