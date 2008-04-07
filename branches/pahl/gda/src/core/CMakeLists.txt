FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\\\"${CMAKE_INSTALL_PREFIX}/tmp\\\")
ADD_DEFINITIONS(-DDEBUG_MSG)

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
	    filelist.h
	    attriblist.h
            gnode.h
            inode.h
            judge.h
            operator.h
            operatorlist.h
            semnet.h
            snode.h
            task.h)

SET(classheaders 
  TreeGNode
  TreeNode
  GNode
  SNode
  Attribute
  AttributeDictionary
  AtribList
  GeoImage
  GeoImageList
  GeoImageCache
  FileList
  Task
  Operator
  OperatorList
  CleanUp
  Analysis
  )

SET(mocheaders  analysis.h
            fatalerror.h
            geoimagecache.h
            judge.h
            task.h)

SET(sources 
	   analysis.cpp
           attribute.cpp
           cleanup.cpp
           fatalerror.cpp
           geoimage.cpp
           geoimagecache.cpp
           geoimagelist.cpp
	   filelist.cpp
           gnode.cpp
           inode.cpp
           judge.cpp
           operator.cpp
           operatorlist.cpp
           semnet.cpp
           snode.cpp
           task.cpp)

QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(gdacore ${sources} ${moclib})

INSTALL(TARGETS gdacore DESTINATION lib)
#INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
