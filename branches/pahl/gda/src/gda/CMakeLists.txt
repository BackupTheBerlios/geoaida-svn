FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DTMPDIR=\\\"${CMAKE_INSTALL_PREFIX}/tmp\\\")
ADD_DEFINITIONS(-DPRGDIR=\\\"${CMAKE_INSTALL_PREFIX}\\\")

INCLUDE_DIRECTORIES(.
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/src/gui
#        ${PROJECT_SOURCE_DIR}/src/semnetgui
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
	${PROJECT_BINARY_DIR}/src/gda
        ${QT_INCLUDES}
)

SET(headers mainwindow.h)

SET(mocheaders mainwindow.h)

SET(classheaders MainWindow)

SET(sources main.cpp mainwindow.cpp)

SET(uifiles mainwindow.ui)

SET(resources gda.qrc)


QT4_WRAP_CPP(moclib ${mocheaders})
QT4_WRAP_UI(uilib ${uifiles})
QT4_ADD_RESOURCES(resourcelib ${resources})


ADD_EXECUTABLE(gda ${sources} ${moclib} ${uilib} ${resourcelib} )


TARGET_LINK_LIBRARIES(gda gdacore gdagui mlparser pfm pnm ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY})

INSTALL(TARGETS gda DESTINATION bin)

