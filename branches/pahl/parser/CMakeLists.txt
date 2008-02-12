PROJECT(mlparser)

FIND_PACKAGE(Qt4 REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(.
	${CMAKE_BINARY_DIR}
        ${QT_INCLUDES}
)

SET(sources
  MLParser.cpp
)

SET(headers
  MLParser.h
)

#SET(uifiles )

#SET(resources)

#QT4_WRAP_CPP(moclib ${headers})
#QT4_WRAP_UI(uilib ${uifiles})
#QT4_ADD_RESOURCES(resourcelib ${resources})

#ADD_LIBRARY(mlparser ${sources} ${moclib} ${uilib} ${resourcelib})
ADD_LIBRARY(mlparser ${sources} ${moclib})

ADD_EXECUTABLE(test_mlparser test_MLParser.cpp ${mlparser})
TARGET_LINK_LIBRARIES(test_mlparser mlparser ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})

ADD_EXECUTABLE(convert_xml convert_xml.cpp)
TARGET_LINK_LIBRARIES(convert_xml ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})


INSTALL(TARGETS mlparser DESTINATION lib)


#TARGET_LINK_LIBRARIES(mlparser
#			        ${QT_QTCORE_LIBRARY}
#				${QT_QTGUI_LIBRARY}
#                                ${QT_LIBRARIES}
#)



