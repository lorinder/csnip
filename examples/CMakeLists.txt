set(samples_c
	clopts.c
	coro.c
	dlist.c
	fmt.c
	getopt.c
	meanvar.c
	sort_cmdline.c
	toy_printf.c
)
if (BUILD_CXX_PIECES)
	set(samples_cxx
		find.cc
		sort_perf.cc
	)
else()
	set(samples_cxx)
endif()

foreach(fn
	${samples_c}
	${samples_cxx}
)
	get_filename_component(tgt "${fn}" NAME_WE)
	add_executable(${tgt} ${fn})
	target_link_libraries(${tgt} csnip)
endforeach()
