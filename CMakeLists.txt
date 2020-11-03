cmake_minimum_required(VERSION 3.10)

project(csnip
	VERSION 0.1
	LANGUAGES C
)

### Check for C++
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

### Project-wide settings and compiler flags

#set(CMAKE_C_VISIBILITY_PRESET hidden)

if (CMAKE_C_COMPILER_ID MATCHES GNU OR CMAKE_C_COMPILER_ID MATCHES CLANG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif ()
if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
  CMAKE_CXX_COMPILER_ID MATCHES CLANG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif ()

### Dependencies
option(BUILD_DOCS "Build documentation" ON)
if (BUILD_DOCS)
	find_package(Doxygen REQUIRED)
endif ()

### Testing

enable_testing()

#### Installation

include(GNUInstallDirs)
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}
	CACHE STRING "Relative installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR}
	CACHE STRING "Relative installation directory for binaries")
set(INSTALL_INCLUDEDIR
	${CMAKE_INSTALL_INCLUDEDIR}/csnip-${PROJECT_VERSION}
  	CACHE STRING "Relative installation directory for headers")
set(INSTALL_DOCDIR
	${CMAKE_INSTALL_DOCDIR}
	CACHE STRING "Relative installation directory for documentation")

#### Subdirectories

add_subdirectory(examples)
add_subdirectory(in)
add_subdirectory(src)
add_subdirectory(test)

# doc/ comes last as Doxygen will need to know the complete other
# targets.
add_subdirectory(doc)


### Packaging

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "lminder@gmx.net")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C snippet library")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
if(NOT DEFINED CPACK_SYSTEM_NAME)
	set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
endif()
set(CPACK_SOURCE_IGNORE_FILES ${PROJECT_BINARY_DIR} .git .gitignore)
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_GENERATOR "TGZ")

include(CPack)
