project(ImageClient)

cmake_minimum_required(VERSION 2.6)

include(FindQt4)
set(QT_USE_QTNETWORK 1)

include_directories(
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
	..
	.
)

set(lib_sources
	image_client.cpp
)
set(exe_sources
	main.cpp
)

set(moc_headers
	image_client.h
)
set(inst_headers
	ImageClient
)

if(QT_WRAP_CPP)
	qt_wrap_cpp(ImageClient lib_sources ${moc_headers})
endif(QT_WRAP_CPP)

add_executable(image_client ${exe_sources})
add_library(imageengine SHARED ${lib_sources})

target_link_libraries(image_client
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	imageengine
)

install(TARGETS image_client imageengine
            RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
		)
install(FILES ${moc_headers} ${inst_headers} DESTINATION include)

