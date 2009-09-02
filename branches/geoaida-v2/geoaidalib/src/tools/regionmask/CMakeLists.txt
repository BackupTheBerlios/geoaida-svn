find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(.
        ${PROJECT_SOURCE_DIR}/src/bottomup
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${CMAKE_SOURCE_DIR}/parser
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
	${QT_INCLUDES}
	)

set(sources regionmask.cpp)

set(data regionmask.mod)

add_executable(regionmask ${sources})
target_link_libraries(regionmask 
				 gaimage 
				 mlparser 
				 pfm 
				 pnm 
				 ${QT_QTCORE_LIBRARY} 
				 ${QT_QTXML_LIBRARY})

add_executable(watch_pipe watch_pipe.c)

install(TARGETS regionmask watch_pipe DESTINATION bin)
install(PROGRAMS ga_regionmask_pipe DESTINATION bin)
install(FILES ${data} DESTINATION share/data/modules)

