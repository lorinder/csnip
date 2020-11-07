# Setup related to installer targets

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
