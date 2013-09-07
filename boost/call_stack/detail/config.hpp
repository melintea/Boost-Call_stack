/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */
 
 
#if !defined(BOOST_CALL_STACK_CONFIG_HPP)
#define BOOST_CALL_STACK_CONFIG_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#include <boost/config.hpp>

/*
 * Pretend C++11.
 */ 
#if defined(BOOST_NO_CXX11_NULLPTR)
#  pragma message ("* Note: Compiler is missing nullptr")
#  define nullptr   0
#endif
#if defined(BOOST_NO_CXX11_NOEXCEPT)
#  pragma message ("* Note: Compiler is missing noexcept")
#  define noexcept  throw()
#endif

#endif //#if !defined(BOOST_CALL_STACK_CONFIG_HPP)
