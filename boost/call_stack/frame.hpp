/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_FRAME_HPP)
#define BOOST_CALL_STACK_FRAME_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/platform.hpp>

#include <boost/call_stack/symbol.hpp>

#include <boost/static_assert.hpp>
#include <boost/move/move.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack {

/**
 * Platform agnostic call frame.
 */

class call_frame
    : protected detail::call_frame_impl
{
public:

    call_frame()
        : call_frame_impl(null_address)
    {}

    call_frame(const call_frame_impl& frm)
        : call_frame_impl(frm)
    {}

#if (__cplusplus >= 201103L) 
    call_frame(call_frame&& other) noexcept 
        : call_frame_impl(other)
    {
    }
#endif

    call_frame(const call_frame& other) noexcept
        : call_frame_impl(other)
    {
    }

    call_frame& operator=(call_frame other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_frame& other) noexcept
    {
        call_frame_impl::swap(other);
    }


    /**
     * @return an opaque address useable by symbol resolvers.
     */
    const address_type addr() const noexcept 
    { 
        return call_frame_impl::addr(); 
    }

    bool operator==(call_frame const& other) const noexcept
    {
        return addr() == other.addr(); //FIXME: use impl
    }
};

inline
void swap(call_frame& left, call_frame& right) noexcept
{
    left.swap(right);
}


/*
 *
 */

static const call_frame  null_frame;

/**
 * Binds together a \ref call_frame, a \ref symbol_resolver and 
 * a formatter for the symbol information.
 *
 * @tparam AddrResolver    See \ref symbol_resolver
 * @tparam OutputFormatter See \ref terse_call_frame_formatter, \ref fancy_call_frame_formatter
 */

template < typename AddrResolver
         , typename OutputFormatter
         >
class call_frame_info
{
public:

    typedef AddrResolver           symbol_resolver_type;
    typedef OutputFormatter        call_frame_formatter_type;

    call_frame_info(const call_frame& frame = null_frame) noexcept
        : _frame(frame)
    {}

#if (__cplusplus >= 201103L) 
    call_frame_info(call_frame_info&& other) noexcept 
        : call_frame_info()
    {
        swap(other);
    }
#endif

    call_frame_info(const call_frame_info& other) noexcept 
        : _frame(other._frame) 
    {}

    call_frame_info& operator=(call_frame_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_frame_info& other) noexcept
    {
        std::swap(_frame, other._frame);
    }

    call_frame frame() const 
    {
        return _frame;
    }

    friend inline std::ostream& operator<<(std::ostream& os,
                                           const call_frame_info& frm)
    {
        call_frame_formatter_type:: template print< symbol_resolver_type >(frm._frame, os);
        return os;
    }

    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return s.str();
    }

    bool operator==(call_frame_info const& other) const noexcept
    {
        return _frame == other._frame; 
    }
    bool operator!=(call_frame_info const& other) const noexcept
    {
        return !(_frame == other._frame); 
    }

private:

    call_frame _frame;
}; //call_frame_info

template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(call_frame_info< AddrResolver, OutputFormatter >& left, 
          call_frame_info< AddrResolver, OutputFormatter >& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/**
 * How to format \ref call_frame information and its symbol information.
 */
struct terse_call_frame_formatter
{
    template < typename AddrResolver >
    static void print(call_frame const& frm, std::ostream& os)
    {
        terse_symbol_formatter::print<AddrResolver>(frm.addr(), os);
    }
};


/**
 * How to format \ref call_frame information and its symbol information.
 */
struct fancy_call_frame_formatter
{
    template < typename AddrResolver >
    static void print(call_frame const& frm, std::ostream& os)
    {
        fancy_symbol_formatter::print<AddrResolver>(frm.addr(), os);
    }
};


}} //namespace boost::call_stack


namespace std {


inline
void swap(boost::call_stack::call_frame& left, boost::call_stack::call_frame& right) noexcept
{
    boost::call_stack::swap(left, right);
}


template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(boost::call_stack::call_frame_info< AddrResolver, OutputFormatter >& left, 
          boost::call_stack::call_frame_info< AddrResolver, OutputFormatter >& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    boost::call_stack::swap(left, right);
}


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_FRAME_HPP)
