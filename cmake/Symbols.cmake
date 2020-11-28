# Check for various compile-time symbols.

include(CheckSymbolExists)

# aligned allocation
check_symbol_exists(posix_memalign "stdlib.h" HAVE_POSIX_MEMALIGN)
check_symbol_exists(aligned_alloc "stdlib.h" HAVE_ALIGNED_ALLOC)
check_symbol_exists(memalign "malloc.h" HAVE_MEMALIGN)
check_symbol_exists(_aligned_malloc "malloc.h" HAVE_ALIGNED_MALLOC)

# System V ucontext API
check_symbol_exists(getcontext "ucontext.h" HAVE_GETCONTEXT)

# regular expressions
check_symbol_exists(regcomp "regex.h" HAVE_REGCOMP)

# strerror_r
set(CMAKE_REQUIRED_DEFINITIONS "-D_POSIX_C_SOURCE=200809L" "-D_GNU_SOURCE")
check_symbol_exists(strerror_r "string.h" HAVE_STRERROR_R)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(strerror_s "sec_api/string_s.h" HAVE_STRERROR_S)

# flockfile
check_symbol_exists(flockfile "stdio.h" HAVE_FLOCKFILE)

