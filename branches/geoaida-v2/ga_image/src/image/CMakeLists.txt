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
           gaimageio_libpng.h
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

ADD_LIBRARY(image STATIC ${sources})
INSTALL(TARGETS image DESTINATION lib)
INSTALL(FILES ${headers} DESTINATION include)
INSTALL(FILES ${classheaders} DESTINATION include/Ga)

ADD_EXECUTABLE(test_cache test_cache.cpp)
TARGET_LINK_LIBRARIES(test_cache image pfm netpbm tiff jpeg png)

ADD_EXECUTABLE(test_image test_image.cpp)
TARGET_LINK_LIBRARIES(test_image image pfm netpbm tiff jpeg png)

