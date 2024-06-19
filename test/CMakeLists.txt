set(tests_c
	arr_test0.c
	arr_test1.c
	clopts_test0.c
	err_test0.c
	err_test1.c
	fmt_test0.c
	fnv_hash_test.c
	hashtable_test0.c
	hashtable_test1.c
	heap_test.c
	limits_test.c
	list_test0.c
	log_test0.c
	log_test1.c
	meanvar_test0.c
	mem_test0.c
	ringbuf_test.c
	ringbuf2_test.c
#	rng_mt_test.c
	runif_getf_test.c
	runif_geti_test.c
	time_test1.c
	util_test0.c
	x_asprintf_test.c
	x_getdelim_test0.c
	x_getopt_test0.c
	x_readv_test0.c
	x_strtok_r_test0.c
	x_writev_test0.c
)
if (BUILD_CXX_PIECES)
	set(tests_cxx
		meanvar_test0_cxx.cc
		mem_test0_cxx.cc
	)
else()
	set(tests_cxx)
endif()

foreach(fn
	${tests_c}
	${tests_cxx}
)
	get_filename_component(tgt "${fn}" NAME_WE)
	add_executable(${tgt} ${fn})
	target_link_libraries(${tgt} csnip)

	add_test(
		NAME ${tgt}
		COMMAND $<TARGET_FILE:${tgt}>
	)
endforeach()

set_property(TARGET clopts_test0 PROPERTY C_STANDARD 11)
set_property(TARGET limits_test PROPERTY C_STANDARD 11)
set_property(TARGET runif_getf_test PROPERTY C_STANDARD 11)
set_property(TARGET runif_geti_test PROPERTY C_STANDARD 11)
set_property(TARGET meanvar_test0 PROPERTY C_STANDARD 11)
set_property(TARGET log_test0 PROPERTY C_STANDARD 11)  # XXX: Maybe avoidable.
set_property(TARGET time_test1 PROPERTY C_STANDARD 11)
