FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\"${CMAKE_INSTALL_PREFIX}/tmp\")
ADD_DEFINITIONS(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
        ${PROJECT_SOURCE_DIR}/src/gaimage
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
#	${CMAKE_BINARY_DIR}
        ${QT_INCLUDES}
)


SET(headers 
           gasplit2regionst.h
           garegionsplittert.h
           garegionsplitter.h
	   garegionsplitter_multiclass.h	
)

set(templateheaders
           gasplit2regionst.hpp
           garegionsplittert.hpp
)

#SET(classheaders   )

#SET(mocheaders  )

SET(sources
    garegionsplitter.cpp
    garegionsplitter_multiclass.cpp		   
)

ADD_LIBRARY(garegionsplitter STATIC ${sources})
install(TARGETS garegionsplitter DESTINATION lib)
install(FILES ${headers} DESTINATION include)
install(FILES ${templateheaders} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
