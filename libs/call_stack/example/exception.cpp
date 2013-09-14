/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


//[exception
/*`
[h4 A traced exception example]
To trace an exception back to the place in code where it was thrown, embed a 
call_stack in the exception.  This might not be feasible nor desireable for 
all types of exceptions.  

For instance, a symbol resolver will usually allocate heap memory for the 
symbol information and this might not be a good thing for in a bad_alloc 
context.  In such difficult situations, the null_resolver might be of help.
*/

#include <iostream>
#include <exception>
#include <boost/call_stack/call_stack.hpp>

class traced_exception : public std::exception
{
public:

    static const std::size_t max_stack_size = 20;
    typedef boost::call_stack::call_stack<max_stack_size> stack_type;
    typedef boost::call_stack::call_stack_info< stack_type
                                              , boost::call_stack::extended_symbol_resolver    // extended_symbol_resolver, basic_symbol_resolver or null_symbol_resolver
                                              , boost::call_stack::fancy_call_frame_formatter  // fancy_call_frame_formatter or terse_call_frame_formatter
                                              >     call_stack_info_type;

    traced_exception()
        : std::exception()
        , _where(true) // Capture stack
    {}

    const stack_type& where() const throw()
    {
        return _where;
    }

    const char * what() const throw()
    {
        return "traced_exception";
    }

private:

    stack_type _where;
};

void func2()
{
    throw traced_exception();
}

void func1()
{
    func2();
}

int main()
{
    try
    {
        func1();
    }
    catch (const traced_exception& ex)
    {
        std::cout << "Exception: " << ex.what() << "\n"
                  << "Stack is " << ex.where().depth() << " frames depth:\n"
                  << traced_exception::call_stack_info_type(ex.where())
                  << std::endl;
    }

    return 0;
}

/*`
Possible output on a Linux platform:
``
Exception: traced_exception
Stack is 10 frames depth:
[0x41ae02] unsigned long boost::call_stack::detail::get_stack<boost::call_stack::call_frame, 20ul>(boost::array<boost::call_stack::call_frame, 20ul>&)+0x40
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/../../../boost/call_stack/detail/gnu/stack.hpp:41
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x41a4d4] boost::call_stack::detail::call_stack_impl<boost::call_stack::call_frame, 20ul>::get_stack()+0x18
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/../../../boost/call_stack/detail/call_stack_impl.hpp:119
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x4195e4] boost::call_stack::detail::call_stack_impl<boost::call_stack::call_frame, 20ul>::call_stack_impl(bool)+0x3e
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/../../../boost/call_stack/detail/call_stack_impl.hpp:59
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x41858d] boost::call_stack::call_stack<20ul>::call_stack(bool)+0x23
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/../../../boost/call_stack/stack.hpp:75
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x416ec0] traced_exception::traced_exception()+0x38
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/exception.cpp:20
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x4146fc] func2()+0x1e
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/exception.cpp:39
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x414717] func1()+0x9
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/exception.cpp:45
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x41472c] main+0x13
    At /home/amelinte/work/boost/call_stack/call_stack/libs/call_stack/example/exception.cpp:61
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
[0x7f94667cfea5] __libc_start_main+0xf5
    At ??:0
    In /lib/x86_64-linux-gnu/libc.so.6
[0x4145c9] _start+0x4145c9
    At ??:0
    In ../../../bin.v2/libs/call_stack/example/gcc-4.7/debug/link-static/threading-multi/exception
``
*/
//]

