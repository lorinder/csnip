### Configure csnip

## Step 1: Feature/Environment checks

# Check for various types & APIs in libc and provided
# libraries.

include(CheckSymbolExists)
include(CheckStructHasMember)
include(CheckTypeSize)

# sizeof(size_t)
check_type_size(size_t SIZE_T)
if (SIZE_T)
	set(CSNIP_CONF__SIZEOF_SIZE_T ${SIZE_T})
endif ()
# ssize_t, and type headers
check_type_size(ssize_t SSIZE_T)
if (HAVE_SYS_TYPES_H)
	set(CSNIP_CONF__HAVE_SYS_TYPES_H 1)
endif ()
if (HAVE_STDINT_H)
	set(CSNIP_CONF__HAVE_STDINT_H 1)
endif ()
if (HAVE_SSIZE_T)
	set(CSNIP_CONF__HAVE_SSIZE_T 1)
endif ()

# Check for include files

check_include_file("io.h" CSNIP_CONF__HAVE_IO_H)
check_include_file("unistd.h" CSNIP_CONF__HAVE_UNISTD_H)

# Check for symbols

check_symbol_exists(aligned_alloc "stdlib.h"
	CSNIP_CONF__HAVE_ALIGNED_ALLOC)
check_symbol_exists(_aligned_malloc "malloc.h"
	CSNIP_CONF__HAVE__ALIGNED_MALLOC)
set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists(asprintf "stdio.h"
	CSNIP_CONF__HAVE_ASPRINTF)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(clock_gettime "time.h"
	CSNIP_CONF__HAVE_CLOCK_GETTIME)
check_symbol_exists(flockfile "stdio.h"
	CSNIP_CONF__HAVE_FLOCKFILE)
set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists(fopencookie "stdio.h"
	CSNIP_CONF__HAVE_FOPENCOOKIE)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(funlockfile "stdio.h"
	CSNIP_CONF__HAVE_FUNLOCKFILE)
set(CMAKE_REQUIRED_DEFINITIONS "-D_BSD_SOURCE")
check_symbol_exists(funopen "stdio.h"
	CSNIP_CONF__HAVE_FUNOPEN)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(getc_unlocked "stdio.h"
	CSNIP_CONF__HAVE_GETC_UNLOCKED)
check_symbol_exists(getdelim "stdio.h"
	CSNIP_CONF__HAVE_GETDELIM)
check_symbol_exists(getline "stdio.h"
	CSNIP_CONF__HAVE_GETLINE)
check_symbol_exists(getopt "unistd.h"
	CSNIP_CONF__HAVE_GETOPT)
check_symbol_exists(memalign "malloc.h"
	CSNIP_CONF__HAVE_MEMALIGN)
check_symbol_exists(posix_memalign "stdlib.h"
	CSNIP_CONF__HAVE_POSIX_MEMALIGN)
check_symbol_exists(putc_unlocked "stdio.h"
	CSNIP_CONF__HAVE_PUTC_UNLOCKED)
check_symbol_exists(readv "sys/uio.h"
	CSNIP_CONF__HAVE_READV)
check_symbol_exists(regcomp "regex.h"
	CSNIP_CONF__HAVE_REGCOMP)
set(CMAKE_REQUIRED_DEFINITIONS
	"-D_POSIX_C_SOURCE=200809L" "-D_GNU_SOURCE")
check_symbol_exists(strerror_r "string.h"
	CSNIP_CONF__HAVE_STRERROR_R)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(strerror_s "sec_api/string_s.h"
	CSNIP_CONF__HAVE_STRERROR_S)
check_symbol_exists(strtok_r "string.h"
	CSNIP_CONF__HAVE_STRTOK_R)
check_struct_has_member("struct timespec" tv_sec "time.h"
	CSNIP_CONF__HAVE_TIMESPEC)
check_symbol_exists(timespec_get "time.h"
	CSNIP_CONF__HAVE_TIMESPEC_GET)
check_struct_has_member("struct timeval" tv_sec "sys/time.h"
	CSNIP_CONF__HAVE_TIMEVAL)
check_symbol_exists(writev "sys/uio.h"
	CSNIP_CONF__HAVE_WRITEV)
set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
check_symbol_exists(vasprintf "stdio.h"
	CSNIP_CONF__HAVE_VASPRINTF)
unset(CMAKE_REQUIRED_DEFINITIONS)

set(CSNIP_CONF__HAVE_UNLOCKED_STDIO 0)
if (${CSNIP_CONF__HAVE_FLOCKFILE}
  AND ${CSNIP_CONF__HAVE_FUNLOCKFILE}
  AND ${CSNIP_CONF__HAVE_GETC_UNLOCKED}
  AND ${CSNIP_CONF__HAVE_PUTC_UNLOCKED})
	set(CSNIP_CONF__HAVE_UNLOCKED_STDIO 1)
endif()

# Create the template file
configure_file(csnip_conf.h.in csnip_conf.h)

## Axl build rules

set(public_headers
	${CMAKE_CURRENT_BINARY_DIR}/csnip_conf.h
	arr.h
	cext.h
	clopts.h
	coro.h
	err.h
	find.h
	fmt.h
	hash.h
	heap.h
	limits.h
	list.h
	log.h
	lphash.h
	lphash_table.h
	meanvar.h
	mem.h
	podtypes.h
	preproc.h
	ringbuf.h
	ringbuf2.h
	rng.h
	rng_mt.h
	runif.h
	sort.h
	time.h
	util.h
	x.h
	x_unistd.h
)
set(c_sources
	clopts.c
	err.c
	fnv_hash.c
	log.c
	meanvar.c
	mem.c
	ringbuf2.c
	rng.c
	rng_mt.c
	runif.c
	time.c
	util.c
	x/asprintf.c
	x/clock_gettime.c
	x/fopencookie.c
	x/getdelim.c
	x/getline.c
	x/getopt.c
	x/readv.c
	x/strdup.c
	x/strerror_r.c
	x/strtok_r.c
	x/writev.c
)

set(csnip_link_libs
	Threads::Threads
	${LINK_LIBM}
)

add_library(csnip SHARED
	${public_headers}
	${c_sources}
)
set_property(TARGET csnip PROPERTY
	WINDOWS_EXPORT_ALL_SYMBOLS ON
)
target_link_libraries(csnip ${csnip_link_libs})
target_include_directories(csnip PUBLIC ..)
target_include_directories(csnip PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)

add_library(csnip-static STATIC
	${public_headers}
	${c_sources}
)
target_link_libraries(csnip-static ${csnip_link_libs})
target_include_directories(csnip-static PUBLIC ..)
target_include_directories(csnip-static PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
set_target_properties(csnip-static PROPERTIES
	POSITION_INDEPENDENT_CODE	ON
)

set_target_properties(csnip PROPERTIES
	SOVERSION	1
	VERSION "1.${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	PUBLIC_HEADER "${public_headers}"
)

install(TARGETS csnip csnip-static LIBRARY
	DESTINATION ${INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR}/csnip)
