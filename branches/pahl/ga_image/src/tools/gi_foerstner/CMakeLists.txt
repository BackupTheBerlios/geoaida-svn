include_directories(${gaimage_SOURCE_DIR}/src/image)

set(sources gi_foerstner.cpp)

set(data gi_foerstner.mod)

add_executable(gi_foerstner ${sources})

target_link_libraries(gi_foerstner gaimage pfm netpbm tiff jpeg png)

install(TARGETS gi_foerstner DESTINATION bin)
install(FILES ${data} DESTINATION share/data/modules)