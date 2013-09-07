/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_WIN_SYMBOL_HPP)
#define BOOST_CALL_STACK_WIN_SYMBOL_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif


#include <boost/call_stack/detail/config.hpp>

#include <boost/call_stack/detail/win/dbghelp.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {

/*
 *
 */

typedef DWORD64            address_type;
static address_type        null_address = address_type(0);

typedef DWORD64            delta_type;
static delta_type          null_delta = delta_type(0);


/*
 *
 */

class null_symbol_resolver
{
public:

    null_symbol_resolver(const address_type& addr = null_address) noexcept
    {
        resolve(addr);
    }

    null_symbol_resolver(null_symbol_resolver& other) noexcept 
    {
        resolve(null_address);
        swap(other);
    }
    null_symbol_resolver& operator=(null_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L) 
    null_symbol_resolver(null_symbol_resolver&& other) noexcept 
    {
        resolve(null_address);
        swap(other);
    }
    null_symbol_resolver& operator=(null_symbol_resolver&& other) noexcept
    {
        BOOST_ASSERT(this != &other);
        swap(other);
        return *this;
    }
#endif

    void resolve(const address_type& addr) noexcept
    {
        _addr = addr;
        _binary_name = nullptr;
        _sym_name = nullptr;
        _demangled_sym_name = nullptr;
        _delta = null_delta;
        _source_file_name = nullptr;
        _line_number = 0;
    }

    const address_type   addr() const noexcept              { return _addr; }
    const char*          binary_file() const noexcept       { return safe(_binary_name); }
    const char*          raw_name() const noexcept          { return safe(_sym_name); }
    const char*          demangled_name() const noexcept    { return safe(_demangled_sym_name); }
    delta_type           delta() const noexcept             { return _delta; }
    const char*          source_file() const noexcept       { return safe(_source_file_name); }
    unsigned int         line_number() const noexcept       { return  _line_number; }


    void swap(null_symbol_resolver& other) noexcept
    {
        std::swap(_addr,                 other._addr);
        std::swap(_binary_name,          other._binary_name);
        std::swap(_sym_name,            other._sym_name);
        std::swap(_demangled_sym_name,  other._demangled_sym_name);
        std::swap(_delta,                other._delta);
        std::swap(_source_file_name,     other._source_file_name);
        std::swap(_line_number,          other._line_number);
    }

protected:

    const char* safe(const char* p) const { return (p && *p) ? p : "??"; }

protected:

    address_type  _addr;
    const char*   _binary_name;
    const char*   _sym_name;
    const char*   _demangled_sym_name;
    delta_type    _delta;

    const char*   _source_file_name;
    unsigned int  _line_number;
}; //null_symbol_resolver

inline void swap(null_symbol_resolver& left, null_symbol_resolver& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/*
 *
 */

class basic_symbol_resolver : public null_symbol_resolver
{
public:

    typedef null_symbol_resolver  base_type;

    basic_symbol_resolver(const address_type& addr = null_address) noexcept : base_type(addr)
    {
        resolve(addr);
    }

    basic_symbol_resolver(basic_symbol_resolver& other) noexcept 
        /*: symbol_resolver_base(other._addr)*/
    {
        resolve(null_address);
        swap(other);
    }
    basic_symbol_resolver& operator=(basic_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L) 
    basic_symbol_resolver(basic_symbol_resolver&& other) noexcept 
        /*: symbol_resolver_base()*/
    {
        resolve(null_address);
        swap(other);
    }
    basic_symbol_resolver& operator=(basic_symbol_resolver&& other) noexcept
    {
        BOOST_ASSERT(this != &other);
        swap(other);
        return *this;
    }
#endif

    void resolve(const address_type& addr) noexcept
    {
        _resolve(addr);
    }

    void swap(basic_symbol_resolver& other) noexcept
    {
        base_type::swap(other);
        
        std::swap(_sym_info, other._sym_info);
        _delta                     = static_cast<delta_type>(*_sym_info.sym_displacement());
        _sym_name                  = _sym_info.name();
        _demangled_sym_name        = _sym_info.name();
        other._delta               = static_cast<delta_type>(*other._sym_info.sym_displacement());
        other._sym_name            = other._sym_info.name();
        other._demangled_sym_name  = other._sym_info.name();

        std::swap(_line_info, other._line_info);
        _line_number               = static_cast<unsigned int>(_line_info.line_number());
        _source_file_name          = _line_info.file_name();
        other._line_number         = static_cast<unsigned int>(other._line_info.line_number());
        other._source_file_name    = other._line_info.file_name();

        std::swap(_module_info, other._module_info);
        _binary_name               = _module_info.loaded_module_name();
        other._binary_name         = other._module_info.loaded_module_name();
    }

protected:

    void _resolve(const address_type& addr) noexcept
    {
        base_type::resolve(addr);

        if (addr == null_address)
        {
            return;
        }

        dbghelp::dbghelp_lib_type::instance().resolve(_addr, _sym_info, _line_info, _module_info);

        _delta               = static_cast<delta_type>(*_sym_info.sym_displacement());
        _sym_name            = _sym_info.name();
        _demangled_sym_name  = _sym_info.name();
        _line_number         = static_cast<unsigned int>(_line_info.line_number());
        _source_file_name    = _line_info.file_name();
        _binary_name         = _module_info.loaded_module_name();
    }

protected:

    dbghelp::symbol_info      _sym_info;
    dbghelp::imagehlp_line    _line_info;
    dbghelp::imagehlp_module  _module_info;
}; //basic_symbol_resolver

inline void swap(basic_symbol_resolver& left, basic_symbol_resolver& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/*
 *
 */

class extended_symbol_resolver : public basic_symbol_resolver
{
public:

    typedef basic_symbol_resolver  base_type;

    extended_symbol_resolver(const address_type& addr = null_address) noexcept : base_type(addr)
    {
        resolve(addr);
    }

    extended_symbol_resolver(extended_symbol_resolver& other) noexcept 
        /*: symbol_resolver_base(other._addr)*/
    {
        resolve(null_address);
        swap(other);
    }
    extended_symbol_resolver& operator=(extended_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L) 
    extended_symbol_resolver(extended_symbol_resolver&& other) noexcept 
        /*: symbol_info_base()*/
    {
        resolve(null_address);
        swap(other);
    }
    extended_symbol_info& operator=(extended_symbol_info&& other) noexcept
    {
        BOOST_ASSERT(this != &other);
        swap(other);
        return *this;
    }
#endif

    void resolve(const address_type& addr) noexcept
    {
        base_type::resolve(addr);
    }

    void swap(extended_symbol_resolver& other) noexcept
    {
        base_type::swap(other);
    }
}; //extended_symbol_info


inline void swap(extended_symbol_resolver& left, extended_symbol_resolver& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_WIN_SYMBOL_HPP)
