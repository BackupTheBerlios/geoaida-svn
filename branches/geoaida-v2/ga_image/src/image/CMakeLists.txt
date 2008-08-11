INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm)

SET(headers 
           gacache.h
           gaconvert.h
           gaimage.h
           gaimagebase.h
           gaimageio.h
           gaimaget.h
           gaimageio_libtiff.h
           gaimageio_libjpeg.h
           garegion.h
           garegiont.h
           gashareddata.h
           gashareddatat.h
           gaimaget.hpp
)


SET(sources 
           gaimage.cpp
           gaimageio.cpp
           gacache.cpp
           gacachefile.cpp
           garegion.cpp
)

SET(classheaders
          Image
	  )

ADD_LIBRARY(gaimage STATIC ${sources})
INSTALL(TARGETS gaimage DESTINATION lib)
INSTALL(FILES ${headers} DESTINATION include)
INSTALL(FILES ${classheaders} DESTINATION include/Ga)

ADD_EXECUTABLE(test_cache test_cache.cpp)
TARGET_LINK_LIBRARIES(test_cache gaimage pfm netpbm tiff jpeg)

ADD_EXECUTABLE(test_image test_image.cpp)
TARGET_LINK_LIBRARIES(test_image gaimage pfm netpbm tiff jpeg)

