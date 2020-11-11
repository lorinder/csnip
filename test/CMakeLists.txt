set(tests_c
	arr_test0.c
	coro_test.c
	err_test0.c
	err_test1.c
	fnv_hash_test.c
	hashtable_test0.c
	hashtable_test1.c
	limits_test.c
	list_test0.c
	log_test0.c
	log_test1.c
	mem_test0.c
	ringbuf_test.c
	rng_mt_test.c
	runif_getf_test.c
	runif_geti_test.c
	time_test1.c
)
if (BUILD_CXX_PIECES)
	set(tests_cxx
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
	target_link_libraries(${tgt} csnip m)

	add_test(
		NAME ${tgt}
		COMMAND $<TARGET_FILE:${tgt}>
	)
endforeach()
