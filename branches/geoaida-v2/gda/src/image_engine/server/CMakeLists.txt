project(ImageServer)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITONS})

set(QT_USE_QTNETWORK 1)

include_directories(
	${gda_SOURCE_DIR}/src/core
	${mlparser_SOURCE_DIR}
	
	${QT_INCLUDES}
	..
	.
)

set(sources
	image_engine_base.cpp
	image_engine_default.cpp
	image_server.cpp
	main.cpp
	../image_engine_commons.cpp
)

set(moc_headers
	image_server.h
)

if(QT_WRAP_CPP)
	qt_wrap_cpp(ImageServer sources ${moc_headers})
endif(QT_WRAP_CPP)

add_executable(image_server ${sources})

target_link_libraries(image_server ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} gdacore mlparser pfm pbm)

install(TARGETS image_server DESTINATION bin)

