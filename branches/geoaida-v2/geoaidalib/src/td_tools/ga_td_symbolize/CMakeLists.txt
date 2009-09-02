find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DGAIMAGE_COMPAT)

include_directories(.
  ${CMAKE_SOURCE_DIR}/pfm/libpfm
  ${CMAKE_SOURCE_DIR}/parser
  ${PROJECT_SOURCE_DIR}/src/bottomup
  ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
  ${QT_INCLUDES}
  )

set(name ga_td_symbolize)
set(sources ${name}.cpp)

set(data ${name}.mod)

add_executable(${name} ${sources})

target_link_libraries(${name} bottomup gaimage mlparser pfm pnm  ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY}  )

install(TARGETS ${name} DESTINATION bin)
install(FILES ${data} DESTINATION share/data/modules)

