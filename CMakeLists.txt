#	/CMakeLists.txt


cmake_minimum_required(VERSION 2.6)


#	Set prefix for installation

if(PREFIX)
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
	message("PREFIX=${PREFIX}")
endif(PREFIX)


#	Check libgtkglext-1.0 >= 1.2.0 and set CFLAGS and LDFLAGS

include(FindPkgConfig)
pkg_check_modules(GTKGLEXT REQUIRED gtkglext-1.0>=1.2.0)
string(REPLACE ";" " " GTKGLEXT_CFLAGS "${GTKGLEXT_CFLAGS}")
string(REPLACE ";" " " GTKGLEXT_LDFLAGS "${GTKGLEXT_LDFLAGS}")


#	Add subdirectories

add_subdirectory(src)
add_subdirectory(gtkbuilder)
add_subdirectory(po)


#	Define data files to install

file(GLOB DATA_FILES *.vs *.fs)
install(FILES ${DATA_FILES} DESTINATION share/gtf)

