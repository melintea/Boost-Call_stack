/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */



#if !defined(BOOST_CALL_STACK_GNU_SYMBOL_HPP)
#define BOOST_CALL_STACK_GNU_SYMBOL_HPP

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif

#include <boost/call_stack/detail/config.hpp>

#include <boost/call_stack/detail/unique.hpp>
#include <boost/call_stack/detail/singleton.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>

#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>

#include <signal.h>

#if !defined(BOOST_CALL_STACK_GNU_NO_LIBBFD)
#  pragma message("Note: Link with optional library libbfd.")
#  include <bfd.h>
#else
#  pragma message("Note: Excluding libbfd. Some symbols might not resolve properly. Line numbers are not available.")
#endif

#include <unistd.h>

#include <cstring>
#include <cstdlib>

#include <memory>
#include <iostream>
#include <fstream>
#include <string> 
#include <map> 
#include <sstream>
#include <algorithm>


/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {

/*
 *
 */

typedef void*                address_type;
static const address_type    null_address = nullptr;

typedef long                 delta_type;
static delta_type            null_delta = delta_type(0);


/*
 *
 */
template <typename T>
struct free_wrapper
{
    void operator() (T*& ptr)
    {
        if (ptr)
        {
            std::free(ptr);
            ptr = nullptr;
        }
    }
};

class null_symbol_resolver
{
public:

    null_symbol_resolver(const address_type& addr = null_address) noexcept
    {
        resolve(addr);
    }

    null_symbol_resolver(null_symbol_resolver const& other) noexcept
    {
        resolve(other._addr);
    }
    null_symbol_resolver& operator=(null_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L)
#pragma message ("**** C++11 FIXME delete this pragma")
    null_symbol_resolver(null_symbol_resolver&& other) noexcept : null_symbol_resolver()
    {
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
        _demangled_sym_name.reset(static_cast<char*>(0), free_wrapper<char>());
        _delta = null_delta;
        _source_file_name = nullptr;
        _line_number = 0;

        _dladdr_ret = false;
    }

    address_type addr() const noexcept              { return _addr; }
    const char*  binary_file() const noexcept       { return safe(_binary_name); }
    const char*  raw_name() const noexcept          { return safe(_sym_name); }
    const char*  demangled_name() const noexcept
    {
        if (_demangled_sym_name) {
            return safe(const_cast<const char*>(_demangled_sym_name.get()));
        }
        else {
            return raw_name();
        }
    }
    delta_type   delta() const noexcept             { return _delta; }
    const char*  source_file() const noexcept       { return safe(_source_file_name); }
    unsigned int line_number() const noexcept       { return  _line_number; }


    void swap(null_symbol_resolver& other) noexcept
    {
        std::swap(_addr,                 other._addr);
        std::swap(_binary_name,          other._binary_name);
        std::swap(_sym_name,             other._sym_name);
        std::swap(_demangled_sym_name,   other._demangled_sym_name);
        std::swap(_delta,                other._delta);
        std::swap(_source_file_name,     other._source_file_name);
        std::swap(_line_number,          other._line_number);
        std::swap(_dladdr_ret,           other._dladdr_ret);
    }

protected:

    const char* safe(const char* p) const { return (p && *p) ? p : "??"; }

protected:

    address_type  _addr;
    const char*   _binary_name;
    const char*   _sym_name;
    boost::shared_ptr<char>  _demangled_sym_name; // Demangler uses malloc()
    delta_type    _delta;

    const char*   _source_file_name;
    unsigned int  _line_number;

    bool          _dladdr_ret;
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

    basic_symbol_resolver(const address_type& addr = nullptr) noexcept : base_type(addr)
    {
        resolve(addr);
    }

    basic_symbol_resolver(basic_symbol_resolver const& other) noexcept : base_type(other._addr)
    {
        resolve(other._addr);
    }
    basic_symbol_resolver& operator=(basic_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L)
    basic_symbol_resolver(basic_symbol_resolver&& other) noexcept : basic_symbol_resolver()
    {
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
        std::swap(_info, other._info);
    }

protected:

    void _demangle() noexcept
    {
        int status = 0;
        _demangled_sym_name.reset(abi::__cxa_demangle(_sym_name, 0, 0, &status),
                                  free_wrapper<char>());
    }

    void _resolve(const address_type& addr) noexcept
    {
        base_type::resolve(addr);

        if (addr == null_address)
            return;

        _dladdr_ret = (::dladdr(_addr, &_info) != 0);
        if (_dladdr_ret)
        {
            _binary_name = _info.dli_fname;
            _sym_name   = _info.dli_sname;
            _delta = static_cast<const char *>(_addr) - static_cast<const char *>(_info.dli_saddr);

            _demangle();
        }
    }

protected:

    Dl_info     _info;
}; //basic_symbol_resolver

inline void swap(basic_symbol_resolver& left, basic_symbol_resolver& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


/*
 *
 */

#if !defined(BOOST_CALL_STACK_GNU_NO_LIBBFD)
namespace bfd {

typedef struct ::bfd cbfd;
struct sym_tab_type
{
    boost::shared_ptr< cbfd >       abfd;
    unsigned                        storage_needed;
    boost::shared_ptr< asymbol* >   syms;
    long                            cSymbols;
    asection *                      text;
    bool                            dynamic;
    boost::shared_ptr< asymbol >    ptr;
    bfd_vma                         base; ///< module base address

    sym_tab_type()
        : storage_needed(0)
        , cSymbols(0)
        , text(nullptr)
        , dynamic(false)
        , base(0)
    {}
};

typedef std::map<std::string, sym_tab_type> sym_map_type;


struct bfd_close_wrapper
{
    bool operator() (cbfd*& abfd)
    {
        bool ret = false;
        if (abfd)
        {
            ret = bfd_close(abfd);
        }
        return ret;
    }
};



class library : public boost::call_stack::detail::unique
{
public:

    library()
        : _init_called(false)
    {
        bool ret = init();
        BOOST_ASSERT(ret);
    }

    ~library()
    {
        bool ret = shutdown();
        BOOST_ASSERT(ret);
    }

    bool init()
    {
        boost::mutex::scoped_lock lock(_lib_mutex);
        return _init();
    }

    bool shutdown()
    {
        boost::mutex::scoped_lock lock(_lib_mutex);
        return _shutdown();
    }

    bfd_vma compute_maps_base(const char * binfile)
    {
        pid_t pid = ::getpid();
        std::ifstream mapsFile("/proc/"+std::to_string(pid)+"/maps");
        std::string line;
        std::string const filePath =
            boost::filesystem::canonical(binfile).string();
        int lineNo = 0;
        while (std::getline(mapsFile, line))
        {
            if (boost::algorithm::ends_with(line, filePath))
            {
                if (lineNo == 0) // this is the executable itself
                    return 0;
                size_t const loc = line.find("-");
                if (loc == std::string::npos)
                    return 0;
                std::string const base = "0x" + line.substr(0, loc);
                std::stringstream sstream(base);
                sstream.imbue(std::locale::classic());
                bfd_vma baseDecimal = 0;
                sstream >> std::hex >> baseDecimal;
                return baseDecimal;
            }
            lineNo++;
        }
        return 0;
    }

    void resolve(const address_type&   addr,
                 const char *   binfile,
                 const char **  source_file_name,
                 const char **  func_name,
                 unsigned int * line_number)
    {
        if (!binfile || !*binfile) {
            return;
        }

        *source_file_name = nullptr;
        *line_number      = 0;
        *func_name        = nullptr;

        boost::mutex::scoped_lock lock(_lib_mutex);

        _init();
        if (!_init_called)
        {
            return;
        }

        sym_map_type::iterator itBfd = _syms.find(binfile);
        if (itBfd == _syms.end()) {
            sym_tab_type fbfd;

            fbfd.base = compute_maps_base(binfile);

            //FIXME: char *find_separate_debug_file (bfd *abfd);
            fbfd.abfd.reset(bfd_openr(binfile, 0), bfd_close_wrapper());
            if (!fbfd.abfd) {
                return;
            }
#ifdef BFD_DECOMPRESS // Old libbfd?
            fbfd.abfd->flags |= BFD_DECOMPRESS;
#endif

            // Required
            bfd_check_format(fbfd.abfd.get(), bfd_object);

            fbfd.text = bfd_get_section_by_name(fbfd.abfd.get(), ".text");

            fbfd.storage_needed = bfd_get_symtab_upper_bound(fbfd.abfd.get());
            if (0 == fbfd.storage_needed) {
                fbfd.dynamic = true;
                fbfd.storage_needed = bfd_get_dynamic_symtab_upper_bound(fbfd.abfd.get());
            }

            fbfd.syms.reset(static_cast<asymbol**>(::malloc(fbfd.storage_needed)),
                            free_wrapper<asymbol*>());

            if (fbfd.dynamic) {
                fbfd.cSymbols = bfd_canonicalize_dynamic_symtab(fbfd.abfd.get(), fbfd.syms.get());
            }
            else {
                fbfd.cSymbols = bfd_canonicalize_symtab(fbfd.abfd.get(), fbfd.syms.get());
            }
            BOOST_ASSERT(fbfd.cSymbols >= 0);

            _syms[binfile] = fbfd;
            itBfd = _syms.find(binfile);
        }

        if (itBfd != _syms.end()) {
            const sym_tab_type &stab = itBfd->second;

            bfd_vma vma = bfd_get_section_vma(stab.abfd.get(), stab.text);

            long offset = ((long)addr) - stab.base - vma; //stab.text->vma;
            if (offset > 0) {
                if ( !bfd_find_nearest_line(stab.abfd.get(),
                                            stab.text,
                                            stab.syms.get(),
                                            offset,
                                            source_file_name,
                                            func_name,
                                            line_number)
                    ) {
                    //std::cerr << "libbfd: could not find " << std::hex << addr;
                    *source_file_name = nullptr;
                    *line_number      = 0;
                    *func_name        = nullptr;
                }
                //trace it
            }
        }
    }

private:

    bool _init()
    {
        if (_init_called)
        {
            return true;
        }

        bfd_init();
        _init_called = true;

        return _init_called;
    }

    bool _shutdown()
    {
        if (!_init_called)
        {
            return true;
        }

        _syms.clear();
        //No couterparty to bfd_init()

        _init_called = false;
        return true;
    }

private:

    boost::mutex    _lib_mutex;
    bool            _init_called;
    sym_map_type    _syms;
};

typedef lpt::singleton<bfd::library> bfd_lib_type;


class source_resolver
{
public:

    const char*  name() const          { return _sym_name; }
    const char*  source_file() const   { return _source_file_name; }
    unsigned int line_number() const   { return  _line_number; }

    source_resolver(const address_type& addr, const char * binfile)
        : _source_file_name(nullptr)
        , _sym_name(nullptr)
        , _line_number(0)
    {
        resolve(addr, binfile);
    }

    friend void swap(source_resolver& left, source_resolver& right)
    {
        std::swap(left._source_file_name, right._source_file_name);
        std::swap(left._sym_name,        right._sym_name);
        std::swap(left._line_number,      right._line_number);
    }

protected:

    void resolve(const address_type& addr, const char * binfile)
    {
        return bfd::bfd_lib_type::instance().resolve(addr, binfile, &_source_file_name, &_sym_name, &_line_number);
    }

protected:

    const char*    _source_file_name;
    const char*    _sym_name;
    unsigned int   _line_number;
};

} //namespace bfd


class extended_symbol_resolver : public basic_symbol_resolver
{
public:

    typedef basic_symbol_resolver  base_type;

    extended_symbol_resolver(const address_type& addr = nullptr) : base_type(addr)
    {
        resolve(addr);
    }

    extended_symbol_resolver(extended_symbol_resolver const& other) noexcept
        : base_type(other._addr)
    {
        resolve(other._addr);
    }

    extended_symbol_resolver& operator=(extended_symbol_resolver other) noexcept
    {
        swap(other);
        return *this;
    }

#if (__cplusplus >= 201103L)
    extended_symbol_resolver(extended_symbol_resolver&& other) noexcept : extended_symbol_resolver()
    {
        swap(other);
    }
    extended_symbol_resolver& operator=(extended_symbol_resolver&& other) noexcept
    {
        BOOST_ASSERT(this != &other);
        swap(other);
        return *this;
    }
#endif


    void resolve(const address_type& addr)
    {
        base_type::resolve(addr);
        _resolve(addr, _binary_name);
    }

    void swap(extended_symbol_resolver& other) noexcept
    {
        base_type::swap(other);
    }

protected:

    void _resolve(const address_type& addr, const char * binfile)
    {
        detail::bfd::source_resolver sym(addr, binfile);
        _line_number        = sym.line_number();
        _source_file_name   = sym.source_file();
        if (!_sym_name && sym.name()) {
            _sym_name = sym.name();
            _demangle();
        }
    }
}; //extended_symbol_resolver

#else //defined(BOOST_CALL_STACK_GNU_NO_LIBBFD)

class extended_symbol_resolver : public basic_symbol_resolver
{
public:

    typedef basic_symbol_resolver  base_type;

    extended_symbol_resolver(const address_type& addr = nullptr) : base_type(addr)
    {
    }
};

#endif // BOOST_CALL_STACK_GNU_NO_LIBBFD

inline void swap(extended_symbol_resolver& left, extended_symbol_resolver& right) noexcept
{
    BOOST_ASSERT(&left != &right);
    left.swap(right);
}


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_GNU_SYMBOL_HPP)
