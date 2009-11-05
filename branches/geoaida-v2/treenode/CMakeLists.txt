FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\"${CMAKE_INSTALL_PREFIX}/tmp\")
ADD_DEFINITIONS(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
#        ${CMAKE_SOURCE_DIR}/3rdparty/netpbm/include
#        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
#	${CMAKE_BINARY_DIR}
        ${QT_INCLUDES}
)


SET(headers 
            gnode.h
	    treenode.h
	    treegnode.h
)

SET(classheaders 
    TreeGNode
    TreeNode
    GNode
  )

#SET(mocheaders  
#    )

SET(sources
    gnode.cpp
    )

#QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(treenode STATIC ${sources})

#INSTALL(TARGETS gdacore DESTINATION lib)
#INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
