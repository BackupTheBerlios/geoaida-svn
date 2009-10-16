project(ImageServer)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK 1)

# include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

include_directories(
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTNETWORK_INCLUDE_DIR}
    ..
)

set(sources
    image_engine_base.cpp
    image_engine_default.cpp
    image_server.cpp
    image_server_thread.cpp
    main.cpp
    ../image_engine_commons.cpp
)

set(moc_headers
    image_server.h
    image_server_thread.h
)

if(QT_WRAP_CPP)
    qt_wrap_cpp(ImageServer sources ${moc_headers})
endif(QT_WRAP_CPP)

add_executable(image_server ${sources})

target_link_libraries(image_server
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
)

install(TARGETS image_server
            RUNTIME DESTINATION bin)


