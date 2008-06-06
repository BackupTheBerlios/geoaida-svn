include_directories(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${PROJECT_SOURCE_DIR}/src/image
	)

set(headers 
	   gageoimage.h
)

set(sources 
	   gageoimage.cpp
)
set(classheaders
          GeoImage
)

add_library(gageoimage STATIC ${sources})
install(TARGETS gageoimage DESTINATION lib)
install(FILES ${headers} DESTINATION include)
install(FILES ${classheaders} DESTINATION include/Ga)




