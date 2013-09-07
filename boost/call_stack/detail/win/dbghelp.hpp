/*
 *  Copyright 2013 Aurelian Melinte. 
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 */



#if !defined(BOOST_CALL_STACK_WIN_DBGHELP_HPP)
#define BOOST_CALL_STACK_WIN_DBGHELP_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1200
#  pragma once
#endif

#if !defined(BOOST_CALL_STACK_HPP)
#  error "This header cannot be included directly. Include <boost/call_stack/call_stack.hpp>"
#endif


#include <boost/call_stack/detail/config.hpp>

#include <boost/call_stack/detail/unique.hpp>
#include <boost/call_stack/detail/singleton.hpp>

#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/array.hpp> 


#include <windows.h>
#include <WinBase.h>
#include <DbgHelp.h>

#include <cstring>
#include <cstdlib>

#include <memory>
#include <iostream>
#include <string> 
#include <map> 
#include <sstream>
#include <algorithm>


#pragma comment(lib, "DbgHelp") // 5.1 or later

/*
 * Supported compilers and tools.  Notes:
 * - VS2003: 
 *   - Favor Small Code (/Os): will wipe out any stack info.
 */

#if (BOOST_MSVC < 1310)
#  error "Unsupported compiler: only Visual Studio 7.1/2003 or later." 
#endif

#if (BOOST_MSVC == 1310)  //VS2003
extern "C" {
    NTSYSAPI VOID NTAPI RtlCaptureContext( PCONTEXT ContextRecord );
}
#endif


/*
 * Supported OSs. Only tested on Win7.
 */

#if (WINVER < _WIN32_WINNT_WIN7) //0x0601
#  error "Unsupported Windows version."
#endif

/*
 *
 */ 

namespace boost { namespace call_stack { namespace detail {

    /*
 * Supported CPUs and tools. 
 */

enum supported_platforms
{
    x86    = IMAGE_FILE_MACHINE_I386,
    amd64  = IMAGE_FILE_MACHINE_AMD64, 
    ia64   = IMAGE_FILE_MACHINE_IA64,
};
typedef unsigned long supported_platform_type;
BOOST_STATIC_ASSERT(sizeof(supported_platform_type) >= sizeof(supported_platforms));

#if   defined(_M_IX86 ) //600
static const unsigned long this_platform = x86;
#elif defined( _M_X64 ) //100
static const unsigned long this_platform = amd64;
#elif defined( _M_IA64)
static const unsigned long this_platform = ia64;
#else
#  error "Unsupported platform."
#endif


/*
 *
 */

template <typename T> 
struct platform { 
    static bool const is_x86        = (this_platform == x86);
    static bool const is_amd64      = (this_platform == amd64);
    static bool const is_ia64       = (this_platform == ia64);
    static bool const is_supported  = (is_amd64 || is_x86 || is_ia64);
};

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename boost::enable_if_c<platform<Context>::is_x86, Context>::type &ctx) 
{
    BOOST_STATIC_ASSERT_MSG(this_platform == x86, "Not x86");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset    = ctx.Eip;
    sf.AddrPC.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset = ctx.Ebp;
    sf.AddrFrame.Mode   = AddrModeFlat;
    sf.AddrStack.Offset = ctx.Esp;
    sf.AddrStack.Mode   = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename boost::enable_if_c<platform<Context>::is_amd64, Context>::type &ctx) 
{
    BOOST_STATIC_ASSERT_MSG(this_platform == amd64, "Not AMD64");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset     = ctx.Rip;
    sf.AddrPC.Mode       = AddrModeFlat;
    sf.AddrFrame.Offset  = ctx.Rsp;
    sf.AddrFrame.Mode    = AddrModeFlat;
    sf.AddrStack.Offset  = ctx.Rsp;
    sf.AddrStack.Mode    = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename boost::enable_if_c<platform<Context>::is_ia64, Context>::type &ctx) 
{
    BOOST_STATIC_ASSERT_MSG(this_platform == ia64, "Not ia64");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset     = ctx.StIIP;
    sf.AddrPC.Mode       = AddrModeFlat;
    sf.AddrFrame.Offset  = ctx.IntSp;
    sf.AddrFrame.Mode    = AddrModeFlat;
    sf.AddrBStore.Offset = ctx.RsBSP;
    sf.AddrBStore.Mode   = AddrModeFlat;
    sf.AddrStack.Offset  = ctx.IntSp;
    sf.AddrStack.Mode    = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename boost::enable_if_c<false, Context>::type &ctx) 
{
    throw std::runtime_error("Unsupported platform");
}


/*
 *
 */

namespace dbghelp {

class symbol_info
{
public:
    symbol_info()
    {
        reset();
    }

    void reset()
    {
        ::memset(&_sym, 0, sizeof(_sym));
        SYMBOL_INFO* pSym = reinterpret_cast<SYMBOL_INFO*>(&_sym);
        pSym->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSym->MaxNameLen = MAX_SYM_NAME;

        _sym_displacement = 0xffffffff;
    }

    operator PSYMBOL_INFO()
    {
        return reinterpret_cast<SYMBOL_INFO*>(&_sym);
    }

    const CHAR* name() const
    {
        const SYMBOL_INFO* pSym = reinterpret_cast<const SYMBOL_INFO*>(&_sym);
        return pSym->Name;
    }

    PDWORD64 sym_displacement()
    {
        return &_sym_displacement;
    }

private:
    CHAR     _sym[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + 1];
    DWORD64  _sym_displacement;
};


class imagehlp_line
{
public:

    imagehlp_line()
        :_lineDisplacement(0)
    {
        reset();
    }

    void reset()
    {
        ::memset(&_line, 0, sizeof(IMAGEHLP_LINE64));
        _line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        _lineDisplacement = 0;
    }

    operator PIMAGEHLP_LINE64()
    {
        return &_line;
    }

    DWORD line_number() const
    {
        return _line.LineNumber;
    }

    PDWORD line_displacement()
    {
        return &_lineDisplacement;
    }

    const CHAR* file_name() const
    {
        return _line.FileName;
    }

private:
    IMAGEHLP_LINE64    _line;
    DWORD             _lineDisplacement;
};


class imagehlp_module
{
public:

    imagehlp_module()
    {
        reset();
    }

    void reset()
    {
        ::memset(&_module, 0, sizeof(IMAGEHLP_MODULE64));
        _module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    }

    operator PIMAGEHLP_MODULE64()
    {
        return &_module;
    }

    const CHAR* loaded_module_name() const
    {
        return _module.LoadedImageName;
    }

private:
    IMAGEHLP_MODULE64  _module;
};


class library : public boost::call_stack::detail::unique
{
public:

    library() 
        : _init_err(0)
        , _init_called(false)
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

    int backtrace(DWORD64* buffer, int size)
    {
        boost::mutex::scoped_lock lock(_lib_mutex);

        _init();
        if (!_init_called)
        {
            return 0;
        }
        
        int depth = 0;
        
        // Alternative: call ImageNtHeader() and use machine info from PE header
        CONTEXT ctx = {0};
        ctx.ContextFlags = CONTEXT_FULL;
        ::RtlCaptureContext(&ctx);
        
        STACKFRAME64 sf;
        init_stackframe<CONTEXT>(sf, ctx);
        
        HANDLE proc = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();
        //DWORD dwProcessId = ::GetCurrentProcessId();
        while (depth < size) {
            BOOL stackRet = ::StackWalk64(
                detail::this_platform, 
                proc, 
                thread, 
                &sf, 
                &ctx, 
                NULL, //::ReadProcessMemory,         //::ReadProcessMemoryProc64, 
                NULL, //::SymFunctionTableAccess64,  //::FunctionTableAccessProc64, 
                NULL, //::SymFunctionTableAccess64,  //::GetModuleBaseProc64, 
                NULL  //::TranslateAddressProc64
                );
            if (!stackRet) {
                break;
            }
            if (sf.AddrPC.Offset == sf.AddrReturn.Offset) {
                break; // endless stack?
            }
            if (sf.AddrPC.Offset == 0) {
                break;
            }
            
            //std::cerr << std::dec << _depth << ": " << std::hex << sf.AddrPC.Offset << std::dec << std::endl;
            buffer[depth++] = sf.AddrPC.Offset;
        }
        
        return depth;
    }

    void resolve(const DWORD64&       addr,
                 symbol_info&         sym_info,
                 imagehlp_line&       line_info,
                 imagehlp_module&     module_info)
    {
        sym_info.reset();
        line_info.reset();
        module_info.reset();

        {
            boost::mutex::scoped_lock lock(_lib_mutex);

            _init();
            if (!_init_called)
            {
                return;
            }

            BOOL ret1 = ::SymFromAddr(::GetCurrentProcess(), addr, sym_info.sym_displacement(), sym_info); 
            if (ret1) {
                BOOL ignore2 = ::SymGetLineFromAddr64(::GetCurrentProcess(), addr, line_info.line_displacement(), line_info);
                ignore2 = ignore2;
                BOOL ignore3 = ::SymGetModuleInfo64(::GetCurrentProcess(), addr, module_info);
                ignore3 = ignore3;
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

        DWORD flags = 0;
#ifdef SYMOPT_UNDNAME
        flags |= SYMOPT_UNDNAME;
#endif
#ifdef SYMOPT_DEFERRED_LOADS
        flags |= SYMOPT_DEFERRED_LOADS;
#endif
#ifdef SYMOPT_LOAD_LINES
        flags |= SYMOPT_LOAD_LINES;
#endif
#ifdef SYMOPT_EXACT_SYMBOLS
        flags |= SYMOPT_EXACT_SYMBOLS;
#endif
#ifdef SYMOPT_INCLUDE_32BIT_MODULES
        flags |= SYMOPT_INCLUDE_32BIT_MODULES;
#endif
#ifdef SYMOPT_NO_PROMPTS
        flags |= SYMOPT_NO_PROMPTS;
#endif
        flags = ::SymSetOptions( flags );

        //MUST: before any other DbgHelp func call
        ::SetLastError(0);
        BOOL init = ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
        BOOST_VERIFY(init == TRUE);
        _init_err = ::GetLastError();
        BOOST_ASSERT(_init_err == 0);

        _init_called = (init == TRUE && _init_err == 0);
        return _init_called; 
    }

    bool _shutdown()
    {
        if (!_init_called)
        {
            return true;
        }

        BOOL clean = ::SymCleanup(::GetCurrentProcess());
        DWORD clean_err = ::GetLastError();
        BOOST_VERIFY(clean == TRUE);
        BOOST_VERIFY(clean_err == ERROR_SUCCESS);

        _init_called = false;

        return (clean == TRUE);
    }

private:

    DWORD               _init_err;     ///< Keep the init error around for inspection.
    bool                _init_called;

    // MSDN: All DbgHelp functions, such as StackWalk64, are single threaded. 
    // Therefore, calls from more than one thread to this function will likely
    // result in unexpected behavior or memory corruption. 
    boost::mutex        _lib_mutex;
};

typedef lpt::singleton<dbghelp::library> dbghelp_lib_type;

} //dbghelp


}}} //namespace boost::call_stack::detail


namespace std {


} //namespace std

#endif //#if !defined(BOOST_CALL_STACK_WIN_DBGHELP_HPP)
