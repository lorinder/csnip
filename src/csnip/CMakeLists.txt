## Configure csnip

include(Symbols)

#function(create_csnip_conf_h)
#	# Configure the file
#	configure_file(csnip_conf.h.in csnip_conf.h)
#endfunction()
#create_csnip_conf_h()

## Axl build rules

set(public_headers
#	${CMAKE_CURRENT_BINARY_DIR}/csnip_conf.h
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
	x_asprintf.c
	x_strerror_r.c
)
target_link_libraries(csnip -lm)
target_include_directories(csnip PUBLIC ..)
target_include_directories(csnip PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)

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
