# Prepare building the documentation

option(BUILD_DOCS "Build documentation" ON)
if (BUILD_DOCS)
	find_package(Doxygen REQUIRED)
endif ()
