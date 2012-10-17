#	/src/CMakeLists.txt

cmake_minimum_required (VERSION 2.6)


#	Define file dependency

file(GLOB SOURCE_FILES *.c)
include_directories(../include)
add_executable(gTF ${SOURCE_FILES})


#	Set CFLAGS and LDFLAGS for gtf

add_definitions(-DDATADIR="${CMAKE_INSTALL_PREFIX}/share/gtf")
set_target_properties(gTF PROPERTIES COMPILE_FLAGS ${GTK2_CFLAGS} LINK_FLAGS ${GTK2_LDFLAGS})


#	Define binary to install

install(TARGETS gTF DESTINATION bin)
