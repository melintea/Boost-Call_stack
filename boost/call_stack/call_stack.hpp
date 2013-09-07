/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */

// TODO
// - doc autoindex & reference
// - move constructor & op

/** \mainpage
\code

    Platform Specific                        Platform Independent                    Library User
    Do not use                               Do not use 
    ---------------------------------------------------------------------------------------------
    
    * address_type ................................................................> * opaque boost::call_stack::address_type

    == Symbols ==
                                  .................................................> * boost::call_stack::symbol_resolver <>: from address to symbol name and more
    * null_symbol_resolver .......+                                                     +... boost::call_stack::null_symbol_resolver 
    * basic_symbol_resolver ......+                                                     +... boost::call_stack::basic_symbol_resolver
    * extended_symbol_resolver .../                                                     \... boost::call_stack::extended_symbol_resolver
                                                                                     * symbol information formatters
                                                                                         +... boost::call_stack::terse_symbol_formatter
                                                                                         \... boost::call_stack::fancy_symbol_formatter
                                                                                     * boost::call_stack::symbol_info <SymResolver, SymFormatter>
                                                                                        
    == Call Stack ==

                                             * call_frame_impl (addr, & more) .....> * boost::call_stack::call_frame: address and possibly more
                                                                                     * call frame information formatters
                                                                                         +... boost::call_stack::terse_call_frame_formatter
                                                                                         \... boost::call_stack::fancy_call_frame_formatter
                                                                                     * boost::call_stack::call_frame_info <SymResolver, FrameFormatter>

                                             * call_stack_impl ....................> * boost::call_stack::call_stack <depth>
                                                 - collection of call_frame_impl         - a collection of boost::call_stack::call_frame
                                                                                     * boost::call_stack::call_stack_info <Stack, SymResolver, FrameFormatter>
                                                                                         - a collection of boost::call_stack::call_frame_info <SymResolver, FrameFormatter>

    ---------------------------------------------------------------------------------------------
    Platform Specific                        Platform Independent                    Library User
    Do not use                               Do not use 

\endcode
*/

#if !defined(BOOST_CALL_STACK_HPP)
#define BOOST_CALL_STACK_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

/**
 *  This could affect the level of detail of the functionality of this library.
 */ 
#if !defined(BOOST_CALL_STACK_NO_OPTIONAL_LIBS)
#  pragma message("* Note: #define BOOST_CALL_STACK_NO_OPTIONAL_LIBS to cut dependencies on optional libraries.")
#endif

#include <boost/call_stack/symbol.hpp>
#include <boost/call_stack/frame.hpp>
#include <boost/call_stack/stack.hpp>

namespace boost { namespace call_stack {

/* 
 *  Library defaults. 
 */
typedef extended_symbol_resolver       default_symbol_resolver;
typedef fancy_symbol_formatter         default_symbol_formatter;

typedef fancy_call_frame_formatter     default_call_frame_formatter;

static const std::size_t               default_max_depth = 40;
typedef call_stack<default_max_depth>  default_stack;
typedef call_stack_info< default_stack, 
                         default_symbol_resolver,
                         default_call_frame_formatter >  default_call_stack_info;


/**
 *  Explicitly initialize the library.  Typically no explicit call is needed.
 *  @return true if successful.
 */
bool init()
{
    return boost::call_stack::detail::init();
}

/**
 *  Explicitly de-initialize the library.  Typically no explicit call is needed.
 *  @return true if successful.
 */
bool shutdown()
{
    return boost::call_stack::detail::shutdown();
}

}} //namespace boost::call_stack


#endif //#if !defined(BOOST_CALL_STACK_HPP)
