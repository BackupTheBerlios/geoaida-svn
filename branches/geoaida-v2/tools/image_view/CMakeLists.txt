project(_image_view)
find_package(Qt4 REQUIRED)

include_directories(${gaimage_SOURCE_DIR}/src/image)

include(${QT_USE_FILE})

set(image_view_SRCS main.cpp MainWindow.cpp ImageWidget.cpp ChannelMappingDialog.cpp CBDialog.cpp)

set(image_view_MOC_HDRS MainWindow.h ImageWidget.h ChannelMappingDialog.h CBDialog.h)

QT4_WRAP_CPP(image_view_MOC_SRCS ${image_view_MOC_HDRS})

add_executable(image_view ${image_view_SRCS} ${image_view_MOC_SRCS})

target_link_libraries(image_view ${QT_LIBRARIES} gaimage pfm netpbm tiff)

install(TARGETS image_view DESTINATION bin)
