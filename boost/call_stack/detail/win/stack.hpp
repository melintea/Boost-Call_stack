/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_WIN_STACK_HPP)
#define BOOST_CALL_STACK_WIN_STACK_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/config.hpp>

#include <boost/call_stack/detail/win/symbol.hpp>
#include <boost/call_stack/detail/win/frame.hpp>

#include <boost/static_assert.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {

/*
 *
 */

template < class CallFrame,
           std::size_t MaxDepth >
std::size_t get_stack(boost::array<CallFrame, MaxDepth>& stack)
{
    BOOST_STATIC_ASSERT_MSG((boost::is_base_of<call_frame_impl, CallFrame>::value), "CallFrame must inherit call_frame_impl");
    address_type buffer[MaxDepth] = {0};

    int numFrames = dbghelp::dbghelp_lib_type::instance().backtrace(buffer, MaxDepth);
    BOOST_ASSERT(numFrames > 0 && numFrames <= MaxDepth);
    for (int i=0; i<numFrames; ++i)
    {
        stack[i] = call_frame_impl(buffer[i]);
    }

    return static_cast<std::size_t>(numFrames);
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_WIN_STACK_HPP)
