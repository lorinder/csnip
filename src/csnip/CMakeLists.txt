## Configure csnip

include(CheckSymbolExists)
include(CheckStructHasMember)

function(create_csnip_conf_h)
	# aligned allocation
	check_symbol_exists(posix_memalign "stdlib.h"
		CSNIP_CONF__HAVE_POSIX_MEMALIGN)
	check_symbol_exists(aligned_alloc "stdlib.h"
		CSNIP_CONF__HAVE_ALIGNED_ALLOC)
	check_symbol_exists(memalign "malloc.h"
		CSNIP_CONF__HAVE_MEMALIGN)
	check_symbol_exists(_aligned_malloc "malloc.h"
		CSNIP_CONF__HAVE_ALIGNED_MALLOC)

	# regular expressions
	check_symbol_exists(regcomp "regex.h"
		CSNIP_CONF__HAVE_REGCOMP)

	# strerror_r
	set(CMAKE_REQUIRED_DEFINITIONS
		"-D_POSIX_C_SOURCE=200809L" "-D_GNU_SOURCE")
	check_symbol_exists(strerror_r "string.h"
		CSNIP_CONF__HAVE_STRERROR_R)
	unset(CMAKE_REQUIRED_DEFINITIONS)
	check_symbol_exists(strerror_s "sec_api/string_s.h"
		CSNIP_CONF__HAVE_STRERROR_S)

	# flockfile
	check_symbol_exists(flockfile "stdio.h"
		CSNIP_CONF__HAVE_FLOCKFILE)

	# uio
	check_symbol_exists(writev "sys/uio.h"
		CSNIP_CONF__HAVE_WRITEV)
	check_symbol_exists(readv "sys/uio.h"
		CSNIP_CONF__HAVE_READV)

	# timeval
	check_struct_has_member("struct timeval" tv_sec "sys/time.h"
		CSNIP_CONF__HAVE_TIMEVAL)

	# Create the template file
	configure_file(csnip_conf.h.in csnip_conf.h)
endfunction()
create_csnip_conf_h()

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
	rng.h
	rng_mt.h
	runif.h
	sort.h
	time.h
	util.h
	x.h
)
set(c_sources
	clopts.c
	err.c
	fnv_hash.c
	log.c
	meanvar.c
	mem.c
	rng.c
	rng_mt.c
	runif.c
	time.c
	util.c
	x/asprintf.c
	x/strdup.c
	x/strerror_r.c
)

add_library(csnip SHARED
	${public_headers}
	${c_sources}
)
target_link_libraries(csnip -lm Threads::Threads)
target_include_directories(csnip PUBLIC ..)
target_include_directories(csnip PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)

add_library(csnip-static STATIC
	${public_headers}
	${c_sources}
)
target_link_libraries(csnip-static -lm Threads::Threads)
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
