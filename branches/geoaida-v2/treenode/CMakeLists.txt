FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\"${CMAKE_INSTALL_PREFIX}/tmp\")
ADD_DEFINITIONS(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
	${CMAKE_SOURCE_DIR}/parser
        ${QT_INCLUDES}
)


SET(headers 
            gnode.h
	    treenode.h
	    treegnode.h

	    treenode.hpp
	    treegnode.hpp
)

SET(classheaders 
    TreeGNode
    TreeNode
    GNode
  )


SET(sources
    gnode.cpp
    )

#QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(treenode STATIC ${sources})

INSTALL(TARGETS treenode DESTINATION treenode)
INSTALL(FILES ${headers} DESTINATION include/treenode)
INSTALL(FILES ${classheaders} DESTINATION include/treenode)
