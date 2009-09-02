find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DGAIMAGE_COMPAT)

include_directories(.
        ${PROJECT_SOURCE_DIR}/src/bottomup
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${CMAKE_SOURCE_DIR}/parser
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
	${QT_INCLUDES}
	)

macro(bu_tool name extra_sources)
  set(sources ${name}.cpp ${extra_sources})

  set(data ${name}.op)

  add_executable(${name} ${sources} ${mocsrc})

#  target_link_libraries(${name} bottomup gaimage mlparser pfm pnm  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY})
  target_link_libraries(${name} bottomup gaimage mlparser pfm pnm  ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})

install(TARGETS ${name} DESTINATION bin)
install(FILES ${data} DESTINATION share/data/operators)

#add_custom_command(TARGET ${name} POST_BUILD
#                   COMMAND mkdir -p ${PROJECT_BINARY_DIR}/src/share/data/application
#                   COMMAND cp -a ${data} ${PROJECT_BINARY_DIR}/src/share/data/application
#		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)

endmacro(bu_tool)

