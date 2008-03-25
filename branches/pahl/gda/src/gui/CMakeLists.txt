FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\\\"${CMAKE_INSTALL_PREFIX}/tmp\\\")
ADD_DEFINITIONS(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
#	${CMAKE_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/core
        ${QT_INCLUDES}
)


SET(headers 
  attributemodel.h
  attributeeditor.h
  )

SET(classheaders 
  AttributeModel
  AttributeEditor

  )

SET(mocheaders 
  attributemodel.h
  attributeeditor.h
  )

SET(sources 
  attributemodel.cpp
  attributeeditor.cpp
  )
 
QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(gdagui ${sources} ${moclib})

INSTALL(TARGETS gdagui DESTINATION lib)
#INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
