#	gtkbuilder/CMakeLists.txt

cmake_minimum_required(VERSION 2.6)

file(GLOB DATA_FILES *.xml *.png)
install(FILES ${DATA_FILES} DESTINATION share/gtf/gtkbuilder)


