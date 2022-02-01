# On Windows, place all binaries and DLLs into a common path so the DLLs
# are found when executing the binaries.  This is needed for testing the
# built binaries in the source tree without installation.
#
# On other systems, we leave the default, which places binaries in the
# build tree corresponding to the source locations.
if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-out)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-out)
endif ()
