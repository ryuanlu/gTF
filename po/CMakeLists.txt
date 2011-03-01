#	po/CMakeLists.txt

cmake_minimum_required(VERSION 2.6)


#	For each .po file, run msgfmt to generate .gmo file and install as .mo file 

file(GLOB PO_FILES *.po)

foreach(target ${PO_FILES})


	#	Get filename without extension

	get_filename_component(target_we ${target} NAME_WE)
	
	
	#	Define custom build rule
	
	set(output "${target_we}.gmo")
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${output} DEPENDS ${target} COMMAND msgfmt ${target} -o ${CMAKE_CURRENT_BINARY_DIR}/${output})


	#	Define installation

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${output} DESTINATION share/locale/${target_we}/LC_MESSAGES/ RENAME gtf.mo)
	
	
	#	Add a .gmo file to the target list
	
	set(gmos "${gmos};${CMAKE_CURRENT_BINARY_DIR}/${output}")

endforeach(target)


#	Add targets in the list to target po, and set target all to depend on target po 

add_custom_target(po ALL DEPENDS ${gmos})




