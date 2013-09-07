/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */
 
 
#if !defined(BOOST_CALL_STACK_UNIQUE_HPP)
#define BOOST_CALL_STACK_UNIQUE_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif



namespace boost { namespace call_stack { namespace detail {

// Stricter than boost::noncopyable. 
class unique
{
protected:
   
    // No copy allowed.
    unique() {}
    ~unique() {}

    // No dynamic instantiation.
    static void* operator new (size_t size);
    static void* operator new (size_t size, void* mem);

private:  
      
    unique( const unique& );
    const unique& operator=( const unique& );

#if (__cplusplus >= 201103L) 
    // No move allowed.
    unique( const unique&& );
    const unique& operator=( const unique&& );
#endif
};
  
}}} //namespace

#endif //#if !defined(BOOST_CALL_STACK_UNIQUE_HPP)
