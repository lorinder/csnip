## Check for required functionality

include(CheckSymbolExists)

# aligned allocation
check_symbol_exists(posix_memalign "stdlib.h" HAVE_POSIX_MEMALIGN)
check_symbol_exists(aligned_alloc "stdlib.h" HAVE_ALIGNED_ALLOC)
check_symbol_exists(memalign "malloc.h" HAVE_MEMALIGN)
check_symbol_exists(_aligned_malloc "malloc.h" HAVE_ALIGNED_MALLOC)

# regular expressions
check_symbol_exists(regcomp "regex.h" HAVE_REGCOMP)

# strerror_r
set(CMAKE_REQUIRED_DEFINITIONS "-D_POSIX_C_SOURCE=200809L" "-D_GNU_SOURCE")
check_symbol_exists(strerror_r "string.h" HAVE_STRERROR_R)
unset(CMAKE_REQUIRED_DEFINITIONS)
check_symbol_exists(strerror_s "sec_api/string_s.h" HAVE_STRERROR_S)

# flockfile
check_symbol_exists(flockfile "stdio.h" HAVE_FLOCKFILE)

## Axl build rules

set(public_headers
	arr.h
	cext.h
	clopts.h
	err.h
	find.h
	hash.h
	heap.h
	limits.h
	list.h
	log.h
	lphash.h
	lphash_table.h
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

add_library(csnip SHARED
	${public_headers}
	clopts.c
	err.c
	fnv_hash.c
	log.c
	mem.c
	rng.c
	rng_mt.c
	runif.c
	time.c
	util.c
	x_strerror_r.c
)
target_link_libraries(csnip -lm)
target_include_directories(csnip PUBLIC ..)

# Add feature preprocessor flags
function (add_feature_defs src)
	foreach (m ${ARGN})
		if (${m})
			set_property(SOURCE ${src}
			  APPEND PROPERTY COMPILE_DEFINITIONS ${m})
		endif ()
	endforeach ()
endfunction ()
add_feature_defs(mem.c HAVE_POSIX_MEMALIGN HAVE_ALIGNED_ALLOC HAVE_MEMALIGN
  HAVE_ALIGNED_MALLOC)
add_feature_defs(log.c HAVE_REGCOMP HAVE_FLOCKFILE)
add_feature_defs(x_strerror_r.c HAVE_STRERROR_R HAVE_STRERROR_S)

set_target_properties(csnip PROPERTIES
	SOVERSION	1
	VERSION "1.${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	PUBLIC_HEADER "${public_headers}"
)

install(TARGETS csnip LIBRARY
	DESTINATION ${INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR}/csnip)
