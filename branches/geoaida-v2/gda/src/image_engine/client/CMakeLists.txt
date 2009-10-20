project(ImageClient)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITONS})

set(QT_USE_QTNETWORK 1)

include_directories(
	${QT_INCLUDES}
	..
	.
)

set(lib_sources
	image_client.cpp
	../image_engine_commons.cpp
)

set(exe_sources
	main.cpp
)

set(moc_headers
	image_client.h
)

set(inst_headers
	../image_engine_commons.h
	../ImageEngineCommons
	ImageClient
)

if(QT_WRAP_CPP)
	qt_wrap_cpp(ImageClient lib_sources ${moc_headers})
endif(QT_WRAP_CPP)

add_library(imageengine SHARED ${lib_sources})

#new: from Gilson
target_link_libraries(imageengine ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})

add_executable(image_client ${exe_sources})

#old: from Torsten
#target_link_libraries(image_client ${QT_LIBRARIES} imageengine)
#install(TARGETS imageengine image_client LIBRARY DESTINATION lib RUNTIME DESTINATION bin)

target_link_libraries(image_client ${QT_QTCORE_LIBRARY} imageengine)

install(TARGETS imageengine DESTINATION lib)
install(TARGETS image_client DESTINATION bin)
install(FILES ${moc_headers} ${inst_headers} DESTINATION include)

