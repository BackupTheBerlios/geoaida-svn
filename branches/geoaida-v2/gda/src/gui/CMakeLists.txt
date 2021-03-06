FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_DEFINITIONS(-DTMPDIR=\"${CMAKE_INSTALL_PREFIX}/tmp\")
ADD_DEFINITIONS(-DDEBUG_MSG)

INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/3rdparty/netpbm/include
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
	${CMAKE_SOURCE_DIR}/parser
        ${PROJECT_SOURCE_DIR}/src/core
	${PROJECT_BINARY_DIR}/src/gui
        ${QT_INCLUDES}
#	${CMAKE_BINARY_DIR}
)


SET(headers 
  attributemodel.h
  attributeeditor.h
  attributevalue.h
  attributevaluedelegate.h
  attributevalueeditor.h
  taskviewer.h
  )

SET(classheaders 
  AttributeModel
  AttributeEditor
  AttributeValue
  AttributeValueDelegate
  AttributeValueEditor
  StringEditor
  FormulaEditor
  OperatorListEditor
  ImageListEditor
  BoolEditor
  EnumEditor
  MainWindow
  TaskViewer
  )

SET(mocheaders 
  attributemodel.h
  attributeeditor.h
  attributevaluedelegate.h
  attributevalueeditor.h
  mainwindow.h
  taskviewer.h
  )

SET(sources 
  attributemodel.cpp
  attributeeditor.cpp
  attributevalue.cpp
  attributevaluedelegate.cpp
  attributevalueeditor.cpp
  mainwindow.cpp
  taskviewer.cpp
  )

SET(uifiles mainwindow.ui)

SET(resources gda.qrc)

QT4_ADD_RESOURCES(resourcelib ${resources})
QT4_WRAP_UI(uilib ${uifiles})
QT4_WRAP_CPP(moclib ${mocheaders})

ADD_LIBRARY(gdagui STATIC ${sources} ${moclib} ${uilib} ${resourcelib} )

#INSTALL(TARGETS gdagui DESTINATION lib)
#INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include)
