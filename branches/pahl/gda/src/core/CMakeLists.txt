FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

add_definitions(-DTMPDIR=\\\"${CMAKE_INSTALL_PREFIX}/tmp\\\")
add_definitions(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
#	${CMAKE_BINARY_DIR}
        ${QT_INCLUDES}
)


SET(headers analysis.h
            attribute.h
            cleanup.h
            fatalerror.h
            geoimage.h
            geoimagecache.h
            geoimagelist.h
            gnode.h
            inode.h
            judge.h
            operator.h
            operatorlist.h
            semnet.h
            snode.h
            task.h)

SET(mocheaders  analysis.h
            fatalerror.h
            geoimagecache.h
            judge.h
            semnet.h
            task.h)

SET(sources 
	   analysis.cpp
           attribute.cpp
           cleanup.cpp
           fatalerror.cpp
           geoimage.cpp
           geoimagecache.cpp
           geoimagelist.cpp
           gnode.cpp
           inode.cpp
           judge.cpp
           operator.cpp
           operatorlist.cpp
           semnet.cpp
           snode.cpp
           task.cpp)

QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(core ${sources} ${moclib})

INSTALL(TARGETS core DESTINATION lib)
