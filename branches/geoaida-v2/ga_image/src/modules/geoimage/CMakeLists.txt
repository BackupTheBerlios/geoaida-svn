INCLUDE(UsePkgConfig)
PKGCONFIG(vips-7.14 VIPS_INC VIPS_LIB VIPS_LINK VIPS_CFLAGS)


include_directories(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${VIPS_INC}
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




