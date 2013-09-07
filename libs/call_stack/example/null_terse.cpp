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
//[null_terse
    typedef boost::call_stack::call_stack<39> stack_type;
    std::cout << "I am here: \n" 
              << boost::call_stack::call_stack_info< stack_type
                                                   , boost::call_stack::null_symbol_resolver
                                                   , boost::call_stack::terse_call_frame_formatter
                                                   >(stack_type(true)) 
              << std::endl;
//]
    return 0;
}

#if 0 //doc
//[null_terse_doc
/*` Output: */
I am here: 
[eb1eea] ??+0x0 (??) in ??:0
[eb6f30] ??+0x0 (??) in ??:0
[eb2b1c] ??+0x0 (??) in ??:0
[eb2ab4] ??+0x0 (??) in ??:0
[eb1086] ??+0x0 (??) in ??:0
[eb9689] ??+0x0 (??) in ??:0
[eb97cd] ??+0x0 (??) in ??:0
[765433aa] ??+0x0 (??) in ??:0
[779d9ef2] ??+0x0 (??) in ??:0
[779d9ec5] ??+0x0 (??) in ??:0
//]
#endif
