/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */


#if !defined(BOOST_CALL_STACK_SYMBOL_HPP)
#define BOOST_CALL_STACK_SYMBOL_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/platform.hpp>

#include <boost/static_assert.hpp>
#include <boost/move/move.hpp>


/*
 *
 */ 

namespace boost { namespace call_stack {

/*
 *
 */

typedef detail::address_type        address_type;
static const address_type           null_address = detail::null_address;

typedef detail::delta_type          delta_type;
static const delta_type             null_delta = detail::null_delta;


/**
 *  Adapters for the address of an object or function.  Not for methods,
 *  even less for virtual ones.
 */
template< class T >
address_type addressof(const T& arg) {
    address_type addr(0);
    addr =  reinterpret_cast<address_type>(
               &const_cast<char&>(
                  reinterpret_cast<const volatile char&>(arg)));
    return addr;
}

template< class T >
address_type addressof(const T* arg) {
    address_type addr(0);
    addr =  reinterpret_cast<address_type>(arg);
    return addr;
}

/**
 *  Symbol resolver interface to plaftorm-specific resolvers.
 */

template < typename Resolver >
class symbol_resolver : protected Resolver
{
public:

    symbol_resolver(const address_type addr = null_address) noexcept
        : Resolver(addr)
    {
    }

    /**
     *  Constructors on an arbitrary address of an object or a function. 
     *  Not necessarily a good idea, use with care.  The underlying platform
     *  resolver might be able to resolve the given address or not. 
     *  @see addressof.
     */
    template < typename T >
    symbol_resolver(const T& arg) noexcept
        : Resolver(addressof(arg))
    {
    }

    template < typename T >
    symbol_resolver(const T* arg) noexcept
        : Resolver(addressof(arg))
    {
    }

    symbol_resolver(const symbol_resolver& other) noexcept 
        : Resolver(other.addr())
    {
    }

#if (__cplusplus >= 201103L) 
    symbol_resolver(symbol_resolver&& other) noexcept 
        : Resolver()
    {
        swap(other);
    }
#endif

    symbol_resolver& operator=(symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

    void resolve(const address_type& addr) noexcept
    {
        Resolver::resolve(addr);
    }

    const address_type     addr() const noexcept                     { return Resolver::addr(); }
    const char*            binary_file() const noexcept              { return Resolver::binary_file(); }
    const char*            raw_name() const noexcept                 { return Resolver::raw_name(); }
    const char*            demangled_name() const noexcept           { return Resolver::demangled_name(); }

    /** 
     * Displacement of \ref addr() from symbol address. Some platforms have 
     * only a positive displacement.
     */
    delta_type             delta() const noexcept                    { return Resolver::delta(); }

    const char*            source_file() const noexcept              { return Resolver::source_file(); }
    unsigned int           line_number() const noexcept              { return Resolver::line_number(); }

    void swap(symbol_resolver& other) noexcept
    {
        Resolver::swap(other);
    }


    template < typename OutputFormatter >
    friend inline std::ostream& operator<<(std::ostream& os,
                                           const symbol_resolver& sym)
    {
        OutputFormatter::print(sym, os);
        return os;
    }

    template < typename OutputFormatter >
    std::string as_string() const
    {
        std::ostringstream s;
        OutputFormatter::print(*this, s);
        return s.str();
    }
}; //symbol_resolver


template < typename Resolver > inline
void swap(symbol_resolver<Resolver>& left, 
          symbol_resolver<Resolver>& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/**
 *  Plain symbol info. No memory allocation permitted when resolving the
 *  symbol, hence, typically, not much information made available; typically, 
 *  no symbol information.  Might be useful for tracking down issues in a 
 *  memory-tight situation.
 */
typedef symbol_resolver<detail::null_symbol_resolver> null_symbol_resolver;


/**
 *  Best resolution offered by libraries always installed with the OS.
 */
typedef symbol_resolver<detail::basic_symbol_resolver> basic_symbol_resolver;


/**
 *  Best resolution for the platform.  Might require additional/optional
 *  libraries.  If \def BOOST_CALL_STACK_NO_OPTIONAL_LIBS is defined, this
 *  resolver is equivalent to \ref boost::call_stack::basic_symbol_resolver.
 */
typedef symbol_resolver<detail::extended_symbol_resolver> extended_symbol_resolver;



/**
 * Binds together a \ref symbol_resolver and a formatter for the symbol 
 * information.
 *
 * @tparam AddrResolver    See \ref symbol_resolver
 * @tparam OutputFormatter See \ref terse_symbol_formatter, \ref fancy_symbol_formatter
 */

template < typename AddrResolver
         , typename OutputFormatter
         >
class symbol_info
    : public AddrResolver
{
public:

    typedef AddrResolver           symbol_resolver_type;
    typedef OutputFormatter        symbol_formatter_type;

    symbol_info(const address_type& addr = null_address) noexcept
        : symbol_resolver_type(addr)
    {}

    /**
     *  Constructors on an arbitrary address of an object or a function. 
     *  Not necessarily a good idea, use with care.  The underlying platform
     *  resolver might be able to resolve the given address or not. 
     *  @see addressof.
     */
    template < typename T >
    symbol_info(const T& arg) noexcept
        : symbol_resolver_type(addressof(arg))
    {
    }

    template < typename T >
    symbol_info(const T* arg) noexcept
        : symbol_resolver_type(addressof(arg))
    {
    }

#if (__cplusplus >= 201103L) 
    symbol_info(symbol_info&& other) noexcept 
    {
        swap(other);
    }
#endif

    symbol_info(const symbol_info& other) noexcept 
    {}

    symbol_info& operator=(symbol_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(symbol_info& other) noexcept
    {
        symbol_resolver_type::swap(other);
    }

    friend inline std::ostream& operator<<(std::ostream& os,
                                           const symbol_info& sym)
    {
        symbol_formatter_type:: template print< symbol_resolver_type >(sym, os);
        return os;
    }

    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return s.str();
    }
};

template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(symbol_info< AddrResolver, OutputFormatter >& left, 
          symbol_info< AddrResolver, OutputFormatter >& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/**
 * How to format \ref symbol_info information.
 */
struct terse_symbol_formatter
{
    template < typename SymbolResolver > 
    static void print(SymbolResolver const& sym, std::ostream& os)
    {
        os << "[" << std::hex << sym.addr() << "] "
           << sym.demangled_name()
           << (sym.delta() >= 0 ? '+' : '-')
           << "0x" << std::hex << sym.delta()
           << " (" << sym.binary_file() << ")"
           << " in " << sym.source_file() << ":" << std::dec << sym.line_number()
           ;
    }
};


/**
 * How to format \ref symbol_info information.
 */
struct fancy_symbol_formatter
{
    template < typename SymbolResolver > 
    static void print(SymbolResolver const& sym, std::ostream& os)
    {
        os << "[" << std::hex << sym.addr() << "] "
           << sym.demangled_name()
           << (sym.delta() >= 0 ? '+' : '-')
           << "0x" << std::hex << sym.delta()
           << "\n\tAt " << sym.source_file() << ":" << std::dec << sym.line_number()
           << "\n\tIn " << sym.binary_file()
           ;
    }
};


}} //namespace boost::call_stack


namespace std {

template < typename Resolver > inline
void swap(boost::call_stack::symbol_resolver<Resolver>& left,
          boost::call_stack::symbol_resolver<Resolver>& right) noexcept
{
    boost::call_stack::swap(left, right);
}


template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(boost::call_stack::symbol_info< AddrResolver, OutputFormatter >& left, 
          boost::call_stack::symbol_info< AddrResolver, OutputFormatter >& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}

} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_SYMBOL_HPP)
