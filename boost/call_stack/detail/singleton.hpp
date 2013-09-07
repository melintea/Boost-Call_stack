/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_SINGLETON_HPP)
#define BOOST_CALL_STACK_SINGLETON_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif


namespace lpt { 

template <class T>
class singleton : private T
{
private:
    singleton()
    {
    }

    ~singleton()
    {
    }

public:
    static T& instance()
    {
        static singleton<T> _staticT;
        return(_staticT);
    }
};

} //lpt

#endif //#if !defined(BOOST_CALL_STACK_SINGLETON_HPP)
