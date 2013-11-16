/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#include <iostream>
#include <boost/call_stack/call_stack.hpp>

int main()
{

//[quick
/*` [h4 Another simple usage] */
/*` If the defaults offered by the library are not adequate, choose a symbol
    resolver, a frame formatter and a stack depth to create the appropriate
    [classref boost::call_stack::call_stack_info]: */

    // Maximum depth of the stack to collect.
    static const std::size_t max_stack_size = 20;  

    typedef boost::call_stack::call_stack<max_stack_size> stack_type;
    stack_type here(true); // Just get the stack. Resolve symbols later.

    typedef boost::call_stack::call_stack_info< stack_type
                                              , boost::call_stack::extended_symbol_resolver    /*< One of: [classref boost::call_stack::extended_symbol_resolver], [classref boost::call_stack::basic_symbol_resolver] or [classref boost::call_stack::null_symbol_resolver] >*/
                                              , boost::call_stack::fancy_call_frame_formatter  /*< One of: [classref boost::call_stack::fancy_call_frame_formatter] or [classref boost::call_stack::terse_call_frame_formatter] >*/
                                              >     extended_call_stack_info_type;
    // Resolve symbols and display the information
    std::cout << "Stack is " << here.depth() << " frames depth:\n"
              << extended_call_stack_info_type(here) 
              << std::endl;
//]

    return 0;
}
