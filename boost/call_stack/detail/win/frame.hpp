/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_WIN_FRAME_HPP)
#define BOOST_CALL_STACK_WIN_FRAME_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/config.hpp>

#include <boost/call_stack/detail/win/symbol.hpp>

#include <boost/static_assert.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {

/*
 *
 */

class call_frame_impl
{
public:

    explicit call_frame_impl(const address_type& addr = null_address)
        : _addr(addr)
    {}

#if (__cplusplus >= 201103L) 
    call_frame_impl(call_frame_impl&& other) noexcept 
        : _addr(null_address)
    {
        swap(other);
    }
#endif

    call_frame_impl(const call_frame_impl& other) noexcept
        : _addr(other._addr)
    {
    }

    call_frame_impl& operator=(call_frame_impl other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_frame_impl& other) noexcept
    {
        std::swap(_addr, other._addr);
    }

    const address_type addr() const noexcept { return _addr; }

private:

    // TODO: add other data like: list of arguments, registers.
    address_type _addr;
};

inline
void swap(call_frame_impl& left, call_frame_impl& right) noexcept
{
    left.swap(right);
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_WIN_FRAME_HPP)
