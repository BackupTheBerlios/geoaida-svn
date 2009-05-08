FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DTMPDIR=\\\"${CMAKE_INSTALL_PREFIX}/tmp\\\")
ADD_DEFINITIONS(-DPRGDIR=\\\"${CMAKE_INSTALL_PREFIX}\\\")

INCLUDE_DIRECTORIES(.
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_BINARY_DIR}/src/gui
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
        ${QT_INCLUDES}
)

SET(headers mainwindow.h)

#SET(mocheaders )

#SET(classheaders )

SET(sources main.cpp )




#QT4_WRAP_CPP(moclib ${mocheaders})

ADD_EXECUTABLE(gda ${sources})


TARGET_LINK_LIBRARIES(gda gdagui gdacore mlparser pfm pnm ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY})

INSTALL(TARGETS gda DESTINATION bin)

