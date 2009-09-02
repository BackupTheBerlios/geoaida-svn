find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(.
        ${PROJECT_SOURCE_DIR}/src/bottomup
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${CMAKE_SOURCE_DIR}/parser
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
	${QT_INCLUDES}
	)

set(sources query_nodes.cpp)

set(data query_nodes.mod)

add_executable(query_nodes ${sources})
target_link_libraries(query_nodes
				 gaimage 
				 mlparser 
				 pfm 
				 pnm 
				 ${QT_QTCORE_LIBRARY} 
				 ${QT_QTXML_LIBRARY})

install(TARGETS query_nodes DESTINATION bin)
#install(FILES ${data} DESTINATION share/data/modules)

