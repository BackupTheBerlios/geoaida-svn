project(ImageClient)

cmake_minimum_required(VERSION 2.6)

include(FindQt4)
set(QT_USE_QTNETWORK 1)

include_directories(
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
)

set(sources
	image_client.cpp
	main.cpp
)

set(moc_headers
	image_client.h
)

if(QT_WRAP_CPP)
	qt_wrap_cpp(ImageClient sources ${moc_headers})
endif(QT_WRAP_CPP)

add_executable(image_client ${sources})

target_link_libraries(image_client
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
)

install(TARGETS image_client
            RUNTIME DESTINATION bin)

