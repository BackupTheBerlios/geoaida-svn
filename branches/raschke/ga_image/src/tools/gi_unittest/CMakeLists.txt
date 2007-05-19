include_directories(.
        ${PROJECT_SOURCE_DIR}/src/gaimage
)

set(sources gi_unittest.cpp)

add_executable(gi_unittest ${sources})

target_link_libraries(gi_unittest gaimage pfm pnm)

install(TARGETS gi_unittest DESTINATION bin)
