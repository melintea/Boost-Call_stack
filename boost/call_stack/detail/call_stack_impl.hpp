/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */
 

#if !defined(BOOST_PI_CALL_STACK_IMPL_HPP)
#define BOOST_PI_CALL_STACK_IMPL_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif


#include <boost/call_stack/detail/platform.hpp>

#include <boost/array.hpp> 
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>


/*
 *
 */ 
namespace boost { namespace call_stack { namespace detail {

/*
 *
 */

template < class CallFrame, std::size_t MaxDepth >
class call_stack_impl
{
public:

    typedef std::size_t                                  depth_type;
    typedef boost::array<CallFrame, MaxDepth>            stack_type;

    typedef typename stack_type::size_type               size_type;
    typedef typename stack_type::difference_type         difference_type;
    typedef typename stack_type::value_type              value_type;
    typedef typename stack_type::const_reference         const_reference;
    typedef typename stack_type::const_iterator          const_iterator;
    typedef typename stack_type::const_reverse_iterator  const_reverse_iterator;

    BOOST_STATIC_ASSERT_MSG((boost::is_base_of<call_frame_impl, CallFrame>::value), "CallFrame must inherit call_frame_impl");

    call_stack_impl(bool capture = false) noexcept 
        : _depth(0)
    {
        if (capture)
        {
            get_stack();
        }
    }

#if (__cplusplus >= 201103L) 
    // Only MSVC 2012 CTP supports delegating constructors 
    call_stack_impl(call_stack_impl&& other) noexcept 
        : call_stack_impl()
    {
        swap(other);
    }
#endif

    call_stack_impl(const call_stack_impl& other) noexcept
        : _stack(other._stack)
        , _depth(other._depth)
    {
    }

    call_stack_impl& operator=(call_stack_impl other) noexcept
    {
        swap(other);
        return *this;
    }

    size_type depth()      const noexcept { return _depth; }
    size_type max_depth()  const noexcept { return MaxDepth; }

    bool empty()           const noexcept { return _depth == 0; }

    const_iterator begin()  const { return _stack.cbegin(); }
    const_iterator end()    const { return (begin() + _depth); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    const_reverse_iterator crbegin() const { return _depth ? const_reverse_iterator(&_stack[_depth-1])
                                                           : _stack.crbegin(); }
    const_reverse_iterator crend()   const { _stack.crend(); }
    const_reverse_iterator rbegin()  const { return crbegin(); }
    const_reverse_iterator rend()    const { return crend(); }

    const_reference operator [] (size_type idx) const { return _stack[idx]; }
    const_reference at(size_type idx) const
    {
        if (!(idx < depth())) {
            throw std::out_of_range("Requested index out of range");
        }
        return _stack.at(idx);
    }

    bool operator ==(const call_stack_impl& other) const { return _depth == other._depth && _stack == other._stack; }
    bool operator !=(const call_stack_impl& other) const { return _depth != other._depth || _stack != other._stack; }

    void swap(call_stack_impl& other) noexcept
    {
        std::swap(_stack, other._stack);
        std::swap(_depth, other._depth);
    }


    depth_type get_stack()
    {
        _depth =  detail::get_stack(_stack);
        BOOST_ASSERT(_depth > 0 && _depth <= MaxDepth);
        return _depth;
    }

private:

    stack_type         _stack;
    depth_type         _depth;
}; //call_stack_impl



template < typename CallFrame, std::size_t Size > inline
void swap(call_stack_impl<CallFrame, Size>& left, call_stack_impl<CallFrame, Size>& right) noexcept
{
    left.swap(right);
}


/*
 *
 */

}}} //namespace boost::call_stack::detail


#endif //#if !defined(BOOST_PI_CALL_STACK_IMPL_HPP)
