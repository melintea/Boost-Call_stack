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
//[frame
    boost::call_stack::call_stack<40> astack(true);
    for (std::size_t i = 0; i < astack.size(); ++i) 
    {
        boost::call_stack::call_frame aframe( astack[i] );
        std::cout << boost::call_stack::call_frame_info< boost::call_stack::extended_symbol_resolver    // extended_symbol_resolver, basic_symbol_resolver or null_symbol_resolver
                                                       , boost::call_stack::fancy_call_frame_formatter  // fancy_call_frame_formatter or terse_call_frame_formatter
                                                       > (aframe) 
                  << "\n"
                  ;
    }
//]

    boost::call_stack::call_stack<40>::const_iterator iend = astack.end();
    for (boost::call_stack::call_stack<40>::const_iterator icrt = astack.begin();
         icrt != iend;
         ++icrt)
    {
        boost::call_stack::call_frame aframe( *icrt );
        std::cout << boost::call_stack::call_frame_info< boost::call_stack::extended_symbol_resolver
                                                       , boost::call_stack::fancy_call_frame_formatter
                                                       > (aframe)
                  << "\n"
                  ;
    }

    return 0;
}

