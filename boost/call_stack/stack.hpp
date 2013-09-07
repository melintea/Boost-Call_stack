/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_STACK_HPP)
#define BOOST_CALL_STACK_STACK_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/platform.hpp>

#include <boost/call_stack/symbol.hpp>
#include <boost/call_stack/frame.hpp>

#include <boost/call_stack/detail/call_stack_impl.hpp>

#include <boost/static_assert.hpp>
#include <boost/move/move.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack {


/**
 * Platform-agnostic call stack information. A collection of \ref call_frame.
 * Use \ref call_stack_info to render it in a human-readable form.
 *
 * @tparam MaxDepth dictates the maximum number of frames that can be captured.
 */

template < std::size_t MaxDepth >
class call_stack
    : protected detail::call_stack_impl<call_frame, MaxDepth>
{
public:

    typedef detail::call_stack_impl<call_frame, MaxDepth>  base_type;

    typedef typename base_type::depth_type              depth_type;
    typedef typename base_type::stack_type              stack_type;

    typedef typename base_type::size_type               size_type;
    typedef typename base_type::difference_type         difference_type;

    typedef typename base_type::value_type              value_type;              ///< \ref call_frame in disguise

    // Only const access is permitted
    typedef typename base_type::const_reference         const_reference;         ///< const reference to a \ref call_frame
    typedef typename base_type::const_iterator          const_iterator;          ///< const iterator yielding a \ref call_frame
    typedef typename base_type::const_reverse_iterator  const_reverse_iterator;

    /**
     * By default, the constructor will not capture the run-time call stack when
     * the object is constructed.  
     * 
     * @param capture If 'true', the run-time stack will be captured.
     *
     * @see get_stack() for an alternate way to capure the run-time stack after
     * construction.
     */
    call_stack(bool capture = false) noexcept 
        : base_type(capture)
    {
    }

#if (__cplusplus >= 201103L) 
    call_stack(call_stack&& other) noexcept 
        : call_stack(other)
    {
    }
#endif

    call_stack(const call_stack& other) noexcept
        : base_type(other)
    {
    }

    call_stack& operator=(call_stack other) noexcept
    {
        swap(other);
        return *this;
    }

    /**
     * Depth of the captured run-time stack.
     */
    size_type depth()      const noexcept { return base_type::depth(); }
    size_type size()       const noexcept { return depth(); }

    /**
     * Maximum depth of the call stack. If the run-time call stack is deeper than 
     * max_depth(), the captured information will be truncated.
     */
    size_type max_depth()  const noexcept { return base_type::max_depth(); }
    size_type max_size()   const noexcept { return max_depth(); }
    size_type capacity()   const noexcept { return max_depth(); }

    bool empty()           const noexcept { return base_type::empty(); }

    /**
     *  @return an iterator pointing to a \ref call_frame.
     */
    const_iterator begin()  const { return base_type::begin(); }
    const_iterator end()    const { return base_type::end(); }
    const_iterator cbegin() const { return base_type::cbegin(); }
    const_iterator cend()   const { return base_type::cend(); }

    const_reverse_iterator crbegin() const { base_type::crbegin(); }
    const_reverse_iterator crend()   const { base_type::crend(); }
    const_reverse_iterator rbegin()  const { base_type::rbegin(); }
    const_reverse_iterator rend()    const { base_type::rend(); }

    /**
     *  @return a const reference to a \ref call_frame.
     */
    const_reference operator [] (size_type idx) const 
    { 
        BOOST_STATIC_ASSERT_MSG((boost::is_same<value_type, call_frame>::value), "value_type must be a call_frame");
        return base_type::operator[](idx); 
    }
    const_reference at(size_type idx)           const { return base_type::at(idx); }

    bool operator ==(const call_stack& other) const { return base_type::operator==(other); }
    bool operator !=(const call_stack& other) const { return base_type::operator!=(other); }

    void swap(call_stack& other) noexcept
    {
        base_type::swap(other);
    }

    /**
     * Capture the call stack at the place where get_stack() is called.
     */
    depth_type get_stack() 
    { 
        return base_type::get_stack(); 
    }

}; //call_stack



template < size_t Size > inline
void swap(call_stack<Size>& left, call_stack<Size>& right) noexcept
{
    left.swap(right);
}


/**
 * Binds together a \ref call_stack, a \ref symbol_resolver and 
 * a formatter for the call frame information.  A collection of
 * \ref call_frame_info.
 *
 * @tparam CallStack       See \ref call_stack
 * @tparam AddrResolver    See \ref symbol_resolver
 * @tparam OutputFormatter See \ref terse_call_frame_formatter, \ref fancy_call_frame_formatter
 *
 */

template < typename CallStack
         , typename AddrResolver
         , typename OutputFormatter
         >
class call_stack_info
{
public:

    typedef CallStack              stack_type;
    typedef AddrResolver           symbol_resolver_type;
    typedef OutputFormatter        call_frame_formatter_type;
    typedef call_frame_info< symbol_resolver_type
                           , call_frame_formatter_type> call_frame_info_type;

    // For containers
    call_stack_info() noexcept : _stack(false) {}

    call_stack_info(const stack_type& stack) noexcept 
        : _stack(stack) 
    {}

#if (__cplusplus >= 201103L) 
    call_stack_info(call_stack_info&& other) noexcept 
        : call_stack_info()
    {
        swap(other);
    }
#endif

    call_stack_info(const call_stack_info& other) noexcept 
        : _stack(other._stack) 
    {}

    call_stack_info& operator=(call_stack_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_stack_info& other) noexcept
    {
        std::swap(_stack, other._stack);
    }


    friend inline std::ostream& operator<<(std::ostream& os,
                                           const call_stack_info& stk)
    {
        for (call_stack_info::const_iterator iter = stk.begin();
            iter != stk.end();
            ++iter)
        {
            call_frame_info_type frm(*iter);
            os << frm << "\n";
        }
        os << std::flush;
        return os;
    }

    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return s.str();
    }

    /**
     *  An iterator yielding a \ref call_frame_info.
     */
    class const_iterator
            : public std::iterator< std::bidirectional_iterator_tag
                                  , ptrdiff_t
                                  >
    {
    public:

        const_iterator(const typename stack_type::const_iterator& it)
                : _it(it)
                , _frame_info(*it)
        {}

        bool operator==(const const_iterator& other) const
        {
            return _it == other._it;
        }

        bool operator!=(const const_iterator& x) const
        {
            return !(*this == x);
        }

        const call_frame_info_type& operator*() const
        {
            return _frame_info;
        }
        const call_frame_info_type* operator->() const
        {
            return &_frame_info;
        }

        const_iterator& operator++()
        {
            ++_it;
            _frame_info = call_frame_info_type(*_it);
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++_it;
            _frame_info = call_frame_info_type(*_it);
            return tmp;
        }

        const_iterator& operator--()
        {
            --_it;
            _frame_info = *_it;
            return *this;
        }
        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --_it;
            _frame_info = call_frame_info_type(*_it);
            return tmp;
        }

    private:

        const_iterator();

    private:

        typename stack_type::const_iterator  _it;
        call_frame_info_type                 _frame_info;
    }; //const_iterator

    const_iterator begin()  const  { return const_iterator(_stack.begin()); }
    const_iterator cbegin() const  { return begin(); }
    const_iterator end()    const  { return const_iterator(_stack.end()); }
    const_iterator cend()   const  { return end(); }

private:

    stack_type         _stack;
};

template < typename CallStack
         , typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(call_stack_info<CallStack, AddrResolver, OutputFormatter >& left, 
          call_stack_info<CallStack, AddrResolver, OutputFormatter >& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


}} //namespace boost::call_stack


namespace std {


template < size_t Size > inline
void swap(boost::call_stack::call_stack<Size>& left,
          boost::call_stack::call_stack<Size>& right) noexcept
{
    boost::call_stack::swap(left, right);
}

template < typename CallStack
         , typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(boost::call_stack::call_stack_info<CallStack, AddrResolver, OutputFormatter>& left, 
          boost::call_stack::call_stack_info<CallStack, AddrResolver, OutputFormatter>& right) noexcept
{
    boost::call_stack::swap(left, right);
}


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_STACK_HPP)
