include_directories(.
        ${PROJECT_SOURCE_DIR}/src/gaimage
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
)

set(sources gi_testhelper.cpp)

add_executable(gi_testhelper ${sources})

target_link_libraries(gi_testhelper gaimage pfm netpbm)

install(TARGETS gi_testhelper DESTINATION bin)
