cmake_minimum_required(VERSION 3.10)

project(csnip
	VERSION 0.3
	LANGUAGES C
)

# Add module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)

include(EnableWarnings)
include(CxxSetup)
include(DoxySetup)
include(InstallSetup)
include(BuildDirSetup)

# Disable MSVC secure/nonstdc warnings.
if (MSVC)
	add_compile_definitions(
		_CRT_NONSTDC_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
	)
else ()
	set(LINK_LIBM m)
endif ()

enable_testing()

#set(CMAKE_C_VISIBILITY_PRESET hidden)

#### Subdirectories

add_subdirectory(examples)
add_subdirectory(in)
add_subdirectory(src)
add_subdirectory(test)

# doc/ comes last as Doxygen will need to know the complete other
# targets.
add_subdirectory(doc)

include(Packaging)
