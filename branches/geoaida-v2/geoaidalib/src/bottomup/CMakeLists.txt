find_package(PkgConfig)
find_package(Qt4 REQUIRED)

#pkg_search_module(VIPS REQUIRED vips-7.18)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DGAIMAGE_COMPAT)

include_directories(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${CMAKE_SOURCE_DIR}/qtguiapplication/src/qtparser
        ${CMAKE_SOURCE_DIR}/ga_image-compatible/src/gaimage
        ${CMAKE_SOURCE_DIR}/parser
        ${QT_INCLUDES}
	${VIPS_INCLUDE_DIRS}
	)

set(headers bottomup.h
           relation.h
           sensor.h
           neighbour.h
           node.h
           nodelist.h
           point2d.h
           regionsensor.h
           stack.h
           stackelement.h
           stackelementnodelist.h
           stackelementnumber.h
           stackelementstack.h
           stackelementstring.h
)

set(classheaders
	BottomUp
	Node
	NodeList
	Stack
	StackElement
	StackElementNodeList
	StackElementStack
	StackElementNumber
	StackElementString
	RegionSensor
	Sensor
	Relation
)
set(sources  bottomup.cpp
           relation.cpp
           sensor.cpp
           neighbour.cpp
           node.cpp
           nodelist.cpp
           point2d.cpp
           regionsensor.cpp
           stack.cpp
           stackelement.cpp
           stackelementnodelist.cpp
           stackelementnumber.cpp
           stackelementstack.cpp
           stackelementstring.cpp
)


add_library(bottomup STATIC ${sources})

#TARGET_LINK_LIBRARIES(bottomup ${VIPS_LIB})

install(TARGETS bottomup DESTINATION lib)
install(FILES ${headers} DESTINATION include/BottomUpLib)
install(FILES ${classheaders} DESTINATION include/BottomUpLib)
