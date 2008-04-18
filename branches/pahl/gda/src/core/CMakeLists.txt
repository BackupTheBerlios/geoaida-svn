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


SET(headers 
	    exception.h
	    project.h
	    analysis.h
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
	    netmodel.h
            semanticnet.h
	    instancenet.h
            snode.h
            task.h)

SET(classheaders 
    Exception
    FileIOException
    ImageException
    NodeException
    TreeGNode
    TreeNode
    GNode
    SNode
    NetModel
    SemanticNet
    InstanceNet
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
    Project
    CleanUp
    Analysis
  )

SET(mocheaders  
    analysis.h
    geoimagecache.h
    judge.h
    task.h)

SET(sources
    project.cpp
    exception.cpp 
    analysis.cpp
    attribute.cpp
    cleanup.cpp
    geoimage.cpp
    geoimagecache.cpp
    geoimagelist.cpp
    filelist.cpp
    gnode.cpp
    inode.cpp
    judge.cpp
    operator.cpp
    operatorlist.cpp
    netmodel.cpp
    semanticnet.cpp
    instancenet.cpp
    snode.cpp
    task.cpp)

QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(gdacore ${sources} ${moclib})

INSTALL(TARGETS gdacore DESTINATION lib)
#INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
