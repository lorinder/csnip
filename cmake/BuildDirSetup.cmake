# Place all binaries and libraries into a common directory.
# 
# This is needed on Windows so that the binaries find the DLLs they are
# linked against.  We do the same everywhere else for simplicity and
# uniformity.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-out)
