#	/CMakeLists.txt


cmake_minimum_required(VERSION 2.6)


#	Set prefix for installation

if(PREFIX)
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
	message("PREFIX=${PREFIX}")
endif(PREFIX)


#	Check libgtk+-2.0 >= 2.12.0 and set CFLAGS and LDFLAGS

include(FindPkgConfig)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0>=2.12.0)
string(REPLACE ";" " " GTK2_CFLAGS "${GTK2_CFLAGS}")
string(REPLACE ";" " " GTK2_LDFLAGS "${GTK2_LDFLAGS}")
set(GTK2_CFLAGS "${GTK2_CFLAGS} -Wall")
set(GTK2_LDFLAGS "${GTK2_LDFLAGS} -lGL -lGLU")



#	Add subdirectories

add_subdirectory(src)
add_subdirectory(gtkbuilder)
add_subdirectory(po)


#	Define data files to install

file(GLOB DATA_FILES *.vs *.fs)
install(FILES ${DATA_FILES} DESTINATION share/gtf)

