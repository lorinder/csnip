# Check if a C++ compiler is available and enable it if the user
# enabled it.

include(CheckLanguage)
check_language(CXX)
if (CMAKE_CXX_COMPILER)
	set(has_cxx ON)
else()
	set(has_cxx OFF)
endif()
option(BUILD_CXX_PIECES "Build C++ parts" ${has_cxx})
if (BUILD_CXX_PIECES)
	enable_language(CXX)
endif()
