find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(.
        ${PROJECT_SOURCE_DIR}/src/bottomup
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${CMAKE_SOURCE_DIR}/parser
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/regionsplitter
	${QT_INCLUDES}
	)

macro(td_tool name extra_sources)
  set(sources ${name}.cpp ${extra_sources})

  set(data ${name}.op)

  add_executable(${name} ${sources} ${mocsrc})


  target_link_libraries(${name} gaimage pfm pnm  ${QT_QTCORE_LIBRARY}  )

  install(TARGETS ${name} DESTINATION bin)
  install(FILES ${data} DESTINATION share/data/operators)
endmacro(td_tool)

macro(mod_tool name extra_sources)
  set(sources ${name}.cpp ${extra_sources})

  set(data ${name}.mod)

  add_executable(${name} ${sources} ${mocsrc})


  target_link_libraries(${name} gaimage garegionsplitter pfm pnm  ${QT_QTCORE_LIBRARY}  )

  install(TARGETS ${name} DESTINATION bin)
  install(FILES ${data} DESTINATION share/data/modules)
endmacro(mod_tool)

macro(td_tool_script name)
  install(FILES ${name}.op DESTINATION share/data/operators)
  install(PROGRAMS ${name} DESTINATION bin)
endmacro(td_tool_script)

