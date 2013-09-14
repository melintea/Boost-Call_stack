/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */

//[default
/*`
[h4 One-line usage]
One can simply use the defaults provided by the library.  In the example below, 
the two steps (getting the stack and resolving its symbols) are collapsed into 
one line:
*/

#include <iostream>
#include <boost/call_stack/call_stack.hpp>

int main()
{
    std::cout << "I am here: \n" 
              << boost::call_stack::default_call_stack_info(boost::call_stack::default_stack(true))  // true: capture the call stack
              << std::endl;
    return 0;
}

/*`

On a Windows platform, the output could look like:
``
I am here: 
[181eea] boost::call_stack::detail::get_stack<boost::call_stack::call_frame,40>+0x4a
        At c:\work\boost\callstack\call_stack\boost\call_stack\detail\win\stack.hpp:43
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[187160] boost::call_stack::detail::call_stack_impl<boost::call_stack::call_frame,40>::get_stack+0x10
        At c:\work\boost\callstack\call_stack\boost\call_stack\detail\call_stack_impl.hpp:122
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[182b2c] boost::call_stack::detail::call_stack_impl<boost::call_stack::call_frame,40>::call_stack_impl<boost::call_stack::call_frame,40>+0x2c
        At c:\work\boost\callstack\call_stack\boost\call_stack\detail\call_stack_impl.hpp:59
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[182ac4] boost::call_stack::call_stack<40>::call_stack<40>+0x14
        At c:\work\boost\callstack\call_stack\boost\call_stack\stack.hpp:61
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[181086] main+0x26
        At c:\work\boost\callstack\call_stack\libs\call_stack\example\default.cpp:9
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[189b89] __tmainCRTStartup+0x199
        At f:\dd\vctools\crt_bld\self_x86\crt\src\crtexe.c:536
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[189ccd] mainCRTStartup+0xd
        At f:\dd\vctools\crt_bld\self_x86\crt\src\crtexe.c:377
        In C:\work\boost\callstack\call_stack\bin.v2\libs\call_stack\example\msvc-11.0\debug\link-static\threading-multi\deflt.exe
[765433aa] ??+0x0
        At ??:0
        In ??
[779d9ef2] ??+0x0
        At ??:0
        In ??
[779d9ec5] ??+0x0
        At ??:0
        In ??
``
*/
