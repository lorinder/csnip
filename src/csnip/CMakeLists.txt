## Configure csnip

include(Symbols)
include(CheckStructHasMember)

function(create_csnip_conf_h)
	# Define the variables
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
add_feature_defs(x/strerror_r.c HAVE_STRERROR_R HAVE_STRERROR_S)

set_target_properties(csnip PROPERTIES
	SOVERSION	1
	VERSION "1.${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	PUBLIC_HEADER "${public_headers}"
)

install(TARGETS csnip csnip-static LIBRARY
	DESTINATION ${INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR}/csnip)
