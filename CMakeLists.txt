cmake_minimum_required(VERSION 3.10)

project(csnip
	VERSION 0.2
	LANGUAGES C
)

# Add module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(EnableWarnings)
include(CxxSetup)
include(DoxySetup)
include(InstallSetup)

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
