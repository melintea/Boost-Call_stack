/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_PLATFORM_HPP)
#define BOOST_CALL_STACK_PLATFORM_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif


#include <boost/call_stack/detail/config.hpp>

#if defined(BOOST_MSVC)
#  include <boost/call_stack/detail/win/win.hpp>
#elif defined(BOOST_GCC)
#  include <boost/call_stack/detail/gnu/gnu.hpp>
#else
#  error "Unsupported platform."
#endif

#include <boost/array.hpp>


/*
 * What to implement for a given platform.
 */

namespace boost { namespace call_stack { namespace detail {

// define address_type
class call_frame_impl;

template < class CallFrame,
           std::size_t MaxDepth >
std::size_t get_stack(boost::array<CallFrame, MaxDepth>& stack);

class null_symbol_resolver;
class basic_symbol_resolver;
class extended_symbol_resolver;

bool init();
bool shutdown();

}}} //namespace boost::call_stack::detail


#endif //#if !defined(BOOST_CALL_STACK_PLATFORM_HPP)
