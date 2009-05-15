find_package(PkgConfig)
pkg_search_module(VIPS REQUIRED vips-7.18)

include_directories(
        .
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${VIPS_INCLUDE_DIRS}
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




