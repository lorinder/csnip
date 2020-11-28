function(get_target_sources retvar target_name)
	get_target_property(src_files ${target_name} SOURCES)
	get_target_property(src_dir ${target_name} SOURCE_DIR)
	set(ret "")
	foreach(src ${src_files})
		if(EXISTS "${src_dir}/${src}")
			list(APPEND ret ${src_dir}/${src})
		endif()
	endforeach()
	set(${retvar} ${ret} PARENT_SCOPE)
endfunction()

function(add_docs_target)
	if(NOT BUILD_DOCS)
		return()
	endif()
	set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
	set(DOXYGEN_MAX_INITIALIZER_LINES 3)
	set(DOXYGEN_REPEAT_BRIEF NO)
	set(DOXYGEN_EXAMPLE_PATH examples/)
	set(DOXYGEN_QUIET YES)

	# Get the sources from the csnip library
	get_target_sources(src_csnip csnip)

	# Figure out whether we can use a stamp file
	set(use_stamp "")
	if(CMAKE_MAJOR_VERSION GREATER 3 OR (CMAKE_MAJOR_VERSION GREATER_EQUAL 3
	  AND CMAKE_MINOR_VERSION GREATER_EQUAL 16))
		set(use_stamp "USE_STAMP_FILE")
	endif()

	doxygen_add_docs(docs
		${src_csnip}
		${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md
		ALL
		${use_stamp}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
endfunction()

add_docs_target()

# Installation
if (BUILD_DOCS)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
		DESTINATION ${INSTALL_DOCDIR})
endif ()
