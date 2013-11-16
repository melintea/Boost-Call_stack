/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */

#define BOOST_LIB_DIAGNOSTIC
#include <boost/config.hpp>

#include <boost/call_stack/call_stack.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <functional>



static const std::size_t test_max_stack_size = 39;  
typedef boost::call_stack::call_stack<test_max_stack_size> test_stack_type;
typedef boost::call_stack::call_stack_info< test_stack_type
                                          , boost::call_stack::null_symbol_resolver
                                          , boost::call_stack::terse_call_frame_formatter
                                          >     test_null_call_stack_info_type;
typedef boost::call_stack::call_stack_info< test_stack_type
                                          , boost::call_stack::basic_symbol_resolver
                                          , boost::call_stack::terse_call_frame_formatter
                                          >     test_basic_call_stack_info_type;
typedef boost::call_stack::call_stack_info< test_stack_type
                                          , boost::call_stack::extended_symbol_resolver
                                          , boost::call_stack::fancy_call_frame_formatter
                                          >     test_extended_call_stack_info_type;

static test_stack_type sg_astack;

int call_stack_tests(int p1, const char* p2)
{
    const char* pIgnore = p2; pIgnore = pIgnore; //Compiler warning

    BOOST_STATIC_ASSERT(test_max_stack_size != boost::call_stack::default_max_depth);

    /*
     * Construction & swap
     */
    test_stack_type s1(true);
    test_stack_type s2;
    std:: cout << "\n*** s2 must be empty:\n " << test_extended_call_stack_info_type(s2);
    std:: cout << "\n*** s1:\n" << test_extended_call_stack_info_type(s1) << std::flush;
    BOOST_CHECK( s2.depth() == 0 );
    BOOST_CHECK( s1.depth() > 0 && s1.depth() <= test_max_stack_size );
    BOOST_CHECK( !s1.empty() && s2.empty() );
    BOOST_CHECK( s1.max_depth() == test_max_stack_size );

    std::swap(s1, s2);
    std:: cout << "\n*** s1 must be empty:\n " << test_extended_call_stack_info_type(s1);
    std:: cout << "\n*** s2:\n" << test_extended_call_stack_info_type(s2) << std::flush;
    BOOST_CHECK( s1.depth() == 0 );
    BOOST_CHECK( s2.depth() > 0 && s2.depth() <= test_max_stack_size );
    BOOST_CHECK( !s2.empty() && s1.empty() );

    /*
     * get_stack() in a global
     */
    test_stack_type s3;
    BOOST_CHECK( s3.depth() == 0 );
    BOOST_CHECK( s3.max_depth() == test_max_stack_size );
    s3.get_stack();
    BOOST_CHECK( s3.depth() > 0 && s3.depth() <= test_max_stack_size );
    BOOST_CHECK( !s3.empty() );

    BOOST_CHECK( sg_astack.depth() == 0 );
    BOOST_CHECK( sg_astack.empty() );
    BOOST_CHECK( sg_astack.max_depth() == test_max_stack_size );
    sg_astack.get_stack();
    BOOST_CHECK( sg_astack.depth() > 0 && sg_astack.depth() <= test_max_stack_size );
    BOOST_CHECK( !sg_astack.empty() );
    std::cout << "\n***sg_astack:\n" << test_extended_call_stack_info_type(sg_astack); 


    /*
     * Print tests
     */
    test_stack_type here(true);
    BOOST_CHECK( here.depth() );

    for (size_t i=0; i< s2.size() ; ++i) {
        boost::call_stack::call_frame aframe( s2[i] );
        std::ostringstream            ossfrm;

        ossfrm << boost::call_stack::call_frame_info< boost::call_stack::extended_symbol_resolver
                                                    , boost::call_stack::fancy_call_frame_formatter >(aframe) << "\n"
                ;
        BOOST_CHECK( ossfrm.str().size() > 0); 


        std::cout << "\n*** Frame: " << i << "\n" << ossfrm.str() << "\n";
    }

    std::cout << "**\n[ null_symbol_resolver stack: " << here.depth() << "\n"
              << test_null_call_stack_info_type(here) << "**] Stack\n\n" << std::flush;
    std::ostringstream ossnull;
    ossnull << test_null_call_stack_info_type(here);
    BOOST_CHECK( ossnull.str().size() > 0); 

    std::cout << "**\n[ basic_symbol_resolver stack: " << here.depth() << "\n"
              << test_basic_call_stack_info_type(here) << "**] Stack\n\n" << std::flush;
    std::ostringstream ossbasic;
    ossbasic << test_basic_call_stack_info_type(here);
    BOOST_CHECK( ossbasic.str().size() > 0); 

    std::cout << "**\n[ extended_symbol_resolver stack: " << here.depth() << "\n"
              << test_extended_call_stack_info_type(here) << "**] Stack\n\n" << std::flush;
    std::ostringstream ossext;
    ossext << test_extended_call_stack_info_type(here);
    BOOST_CHECK( ossext.str().size() > 0); 
    BOOST_CHECK( ossext.str().find("call_stack_tests") != std::string::npos);


    boost::call_stack::call_stack<1> one(true);
    BOOST_CHECK( one.depth() == 1 );
    std::cout << "\n*** Stack with depth one: \n" 
              << "depth=" << one.depth() << "\n" 
              << boost::call_stack::call_stack_info< boost::call_stack::call_stack<1>
                                                   , boost::call_stack::extended_symbol_resolver 
                                                   , boost::call_stack::fancy_call_frame_formatter
                                                   >(one)
              ;

    test_stack_type s4 = sg_astack;
    BOOST_CHECK( s4.depth() == sg_astack.depth() && !s4.empty() );

    test_stack_type s5(sg_astack);
    BOOST_CHECK( s5.depth() == sg_astack.depth() && !s5.empty() );

    return p1;
}


class aclass
{
public: //private:
    static void staticm(int p1, const char* p2)
    {
        int ignore = p1; ignore= ignore; //Compiler warning
        std::cout << __FUNCTION__  << std::endl;
        call_stack_tests(p1, p2);
    }

public:
    void meth2(int p1, const char* p2)
    {
        std::cout << __FUNCTION__  << std::endl;
        staticm(p1, p2);
    }

    virtual void vmeth1(int p1)
    {
        std::cout << __FUNCTION__  << std::endl;
        meth2(p1, "vmeth1");
    }
};


int func1(int p1)
{
    std::cout << __FUNCTION__ << std::endl;

    aclass x;
    x.vmeth1(p1);

    return p1;
}

static int staticf(int p1)
{
    return func1(p1);
}


void test_call_stack()
{
    std::cout << "\n*\n* " << __FUNCTION__ << "\n*\n" << std::endl;
    staticf(1);
}

void test_call_stack_info()
{
    std::cout << "\n*\n* " << __FUNCTION__ << "\n*\n" << std::endl;

    /*
     * Construction and swap
     */
    test_stack_type s1(true);
    test_stack_type s2;
    BOOST_CHECK( s1.depth() > 0 && s2.depth() == 0 );

    test_extended_call_stack_info_type i1(s1);
    test_extended_call_stack_info_type i2(s2);
    std::swap(i1, i2);
    BOOST_CHECK( s1.depth() > 0 && s2.depth() == 0 );

    /*
     * Print test
     */
    std::cout << "\ni1: \n" << i1 << std::endl;
    std::cout << "\ni2: \n" << i2 << std::endl;
}



void test_symbol()
{
    std::cout << "\n*\n* " << __FUNCTION__ << "\n*\n" << std::endl;

    // Get a valid address from a stack
    boost::call_stack::call_stack<1> one(true);
    boost::call_stack::symbol_resolver< boost::call_stack::extended_symbol_resolver > sym3( one[0].addr() );
    BOOST_CHECK( sym3.addr() == one[0].addr() );
    BOOST_CHECK( std::string(sym3.binary_file()).find("test") != std::string::npos ); // Binary: test_call_stack
    BOOST_CHECK( std::string(sym3.raw_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( std::string(sym3.demangled_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( sym3.delta() != boost::call_stack::null_delta );
    BOOST_CHECK( std::string(sym3.source_file()).find("stack.hpp") != std::string::npos );
    BOOST_CHECK( sym3.line_number() > 0 );

    std::string outs = sym3.as_string<boost::call_stack::fancy_symbol_formatter>();
    BOOST_CHECK( outs.find("stack.hpp") != std::string::npos );
    std::cout << "\nsymbol:\n" << sym3.as_string<boost::call_stack::fancy_symbol_formatter>() << std::endl;

    boost::call_stack::symbol_resolver< boost::call_stack::extended_symbol_resolver > sym4(sym3);
    BOOST_CHECK( sym3.addr() == one[0].addr() );
    BOOST_CHECK( std::string(sym3.binary_file()).find("test") != std::string::npos ); // Binary: test_call_stack
    BOOST_CHECK( std::string(sym3.raw_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( std::string(sym3.demangled_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( sym3.delta() != boost::call_stack::null_delta );
    BOOST_CHECK( std::string(sym3.source_file()).find("stack.hpp") != std::string::npos );
    BOOST_CHECK( sym3.line_number() > 0 );
    BOOST_CHECK( sym4.addr() == sym3.addr() );
    BOOST_CHECK( sym4.delta() == sym3.delta() );
    BOOST_CHECK( sym4.source_file() == sym3.source_file() );
    BOOST_CHECK( sym4.line_number() == sym3.line_number() );

    boost::call_stack::symbol_resolver< boost::call_stack::extended_symbol_resolver > sym5 = sym3;
    BOOST_CHECK( sym3.addr() == one[0].addr() );
    BOOST_CHECK( std::string(sym3.binary_file()).find("test") != std::string::npos ); // Binary: test_call_stack
    BOOST_CHECK( std::string(sym3.raw_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( std::string(sym3.demangled_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( sym3.delta() != boost::call_stack::null_delta );
    BOOST_CHECK( std::string(sym3.source_file()).find("stack.hpp") != std::string::npos );
    BOOST_CHECK( sym3.line_number() > 0 );
    BOOST_CHECK( sym5.addr() == sym3.addr() );
    BOOST_CHECK( sym5.delta() == sym3.delta() );
    BOOST_CHECK( sym5.source_file() == sym3.source_file() );
    BOOST_CHECK( sym5.line_number() == sym3.line_number() );

    // Construction from some address (a global)
    boost::call_stack::symbol_resolver< boost::call_stack::extended_symbol_resolver > sym1( sg_astack );
    boost::call_stack::symbol_resolver< boost::call_stack::extended_symbol_resolver > sym2;
    BOOST_CHECK( sym2.addr() == boost::call_stack::null_address && sym1.addr() != boost::call_stack::null_address );
    BOOST_CHECK(sym1.raw_name() && sym1.binary_file());
    BOOST_CHECK(std::string(sym1.binary_file()).find("test_call_stack") != std::string::npos);
    std::cout << "\nsg_astack symbol: \n" << sym1.as_string<boost::call_stack::fancy_symbol_formatter>() << std::endl;

    std::swap(sym1, sym2); 
    BOOST_CHECK( sym2.addr() != boost::call_stack::null_address && sym1.addr() == boost::call_stack::null_address );
    BOOST_CHECK(sym2.raw_name() && sym2.binary_file());
    BOOST_CHECK(std::string(sym2.binary_file()).find("test_call_stack") != std::string::npos);
    std::cout << "\nsg_astack symbol: \n" << sym2.as_string<boost::call_stack::terse_symbol_formatter>() << std::endl;
}

void test_symbol_info()
{
    std::cout << "\n*\n* " << __FUNCTION__ << "\n*\n" << std::endl;

    // Get a valid address from a stack
    boost::call_stack::call_stack<1> one(true);
    boost::call_stack::symbol_info< boost::call_stack::extended_symbol_resolver,
                                    boost::call_stack::fancy_symbol_formatter > sym3( one[0].addr() );
    BOOST_CHECK( sym3.addr() == one[0].addr() );
    BOOST_CHECK( std::string(sym3.binary_file()).find("test") != std::string::npos ); // Binary: test_call_stack
    BOOST_CHECK( std::string(sym3.raw_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( std::string(sym3.demangled_name()).find("call_stack") != std::string::npos );
    BOOST_CHECK( sym3.delta() != boost::call_stack::null_delta );
    BOOST_CHECK( std::string(sym3.source_file()).find("stack.hpp") != std::string::npos );
    BOOST_CHECK( sym3.line_number() > 0 );

    std::string outs = sym3.as_string();
    BOOST_CHECK( outs.find("stack.hpp") != std::string::npos );
    std::cout << "\nsymbol info:\n" << sym3 << std::endl; 

    // Construction from some address (a function) MSVC 7.1 fails to resolve it.
    // warning: ISO C++ forbids casting between pointer-to-function and pointer-to-object [enabled by default]
    boost::call_stack::symbol_info< boost::call_stack::extended_symbol_resolver,
                                    boost::call_stack::fancy_symbol_formatter > sym1( func1 );
    boost::call_stack::symbol_info< boost::call_stack::extended_symbol_resolver,
                                    boost::call_stack::fancy_symbol_formatter > sym2;
    BOOST_CHECK( sym2.addr() == boost::call_stack::null_address && sym1.addr() != boost::call_stack::null_address );
    BOOST_CHECK(sym1.raw_name() && sym1.binary_file());
    std::cout << "\nfunc1 symbol info:\n" << sym1 << std::endl; 

    std::swap(sym1, sym2); 
    BOOST_CHECK( sym2.addr() != boost::call_stack::null_address && sym1.addr() == boost::call_stack::null_address );
    BOOST_CHECK(sym2.raw_name() && sym2.binary_file());
    std::cout << "\nfunc1 symbol info:\n" << sym2 << std::endl; 
}

void test_call_frame_info()
{
    std::cout << "\n*\n* " << __FUNCTION__ << "\n*\n" << std::endl;

    boost::call_stack::call_frame f1(test_stack_type(true)[0]); 
    BOOST_CHECK(f1.addr() != boost::call_stack::null_address);
    boost::call_stack::call_frame f2;
    BOOST_CHECK(f2.addr() == boost::call_stack::null_address);

    boost::call_stack::call_frame_info< boost::call_stack::extended_symbol_resolver
                                      , boost::call_stack::fancy_call_frame_formatter >  fi1(f1), fi2(f2);
    std::swap(f1, f2);
    BOOST_CHECK(f2.addr() != boost::call_stack::null_address && f1.addr() == boost::call_stack::null_address);

    boost::call_stack::call_frame f3 = f2;
    BOOST_CHECK(f2.addr() != boost::call_stack::null_address );
    BOOST_CHECK(f3.addr() == f2.addr() );

    boost::call_stack::call_frame f4(f2);
    BOOST_CHECK(f2.addr() != boost::call_stack::null_address );
    BOOST_CHECK(f4.addr() == f2.addr() );

    BOOST_CHECK(fi1.frame().addr() != boost::call_stack::null_address && fi2.frame().addr() == boost::call_stack::null_address);
    std::swap(fi1, fi2);
    BOOST_CHECK(fi2.frame().addr() != boost::call_stack::null_address && fi1.frame().addr() == boost::call_stack::null_address);

    std::string outs = fi2.as_string();
    BOOST_CHECK( outs.find("stack.hpp") != std::string::npos );

    std::cout << "\nfi1: \n" << fi2 << std::endl;
}

void test_start()
{
#if defined(BOOST_MSVC)
    std::cout << "MSVC compiler version: " << _MSC_VER << " running on WINVER: " << WINVER;
# if defined (_DEBUG)
    std::cout << "; Debug build" << std::endl;
# else
    std::cout << "; Release build" << std::endl;
# endif
#elif defined(__GNUG__) || defined(BOOST_GCC)
    std::cout << "gcc version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
#else
#  error "Unsupported platform"
#endif
    std::cout << "__cplusplus version: " << __cplusplus << std::endl;

    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    std::cout << "BOOST_VERSION: " << BOOST_VERSION << " BOOST_PLATFORM: " << BOOST_PLATFORM
              << "\nDate: " << now.date() << " " << now.time_of_day() 
              << "\n=================================================" 
              << std::endl;
}

void test_end()
{
    bool down = boost::call_stack::shutdown();
    BOOST_CHECK(down == true);

    // Shold survive multiple shutdown calls
    down = boost::call_stack::shutdown();
    BOOST_CHECK(down == true);

    std::cout << std::endl << std::endl;

    std::cout << "sizeof(address_type)="                        << sizeof(boost::call_stack::address_type) << std::endl;
    std::cout << "sizeof(symbol_info)="                         << sizeof(boost::call_stack::symbol_info< boost::call_stack::extended_symbol_resolver,
                                                                                                          boost::call_stack::fancy_symbol_formatter >) << std::endl;
    std::cout << "sizeof(extended_symbol_resolver)="            << sizeof(boost::call_stack::extended_symbol_resolver) << std::endl;

    std::cout << "sizeof(call_frame)="                          << sizeof(boost::call_stack::call_frame) << std::endl;
    std::cout << "sizeof(call_frame_info)="                     << sizeof(boost::call_stack::call_frame_info< boost::call_stack::extended_symbol_resolver
                                                                                                            , boost::call_stack::fancy_call_frame_formatter >) << std::endl;
    std::cout << "sizeof(test_stack_type)="                     << sizeof(test_stack_type) << std::endl;
    std::cout << "sizeof(test_null_call_stack_info_type)="      << sizeof(test_null_call_stack_info_type) << std::endl;
    std::cout << "sizeof(test_basic_call_stack_info_type)="     << sizeof(test_basic_call_stack_info_type) << std::endl;
    std::cout << "sizeof(test_extended_call_stack_info_type)="  << sizeof(test_extended_call_stack_info_type) << std::endl;
    std::cout << "=================================================" << std::endl;
}


/*
 *
 */
boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
    boost::unit_test::test_suite* tests =
        BOOST_TEST_SUITE("Boost.Call_stack: Tests for call stack utilities.");

    tests->add(BOOST_TEST_CASE(test_start));

    tests->add(BOOST_TEST_CASE(test_symbol));
    tests->add(BOOST_TEST_CASE(test_symbol_info));
    tests->add(BOOST_TEST_CASE(test_call_frame_info));
    tests->add(BOOST_TEST_CASE(test_call_stack));
    tests->add(BOOST_TEST_CASE(test_call_stack_info));

    tests->add(BOOST_TEST_CASE(test_end));

    return tests;
}
