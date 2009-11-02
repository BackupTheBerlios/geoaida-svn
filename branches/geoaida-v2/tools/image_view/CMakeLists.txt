project(_image_view)
cmake_minimum_required(VERSION 2.4)

FIND_PACKAGE(Qt4 REQUIRED)

FIND_PACKAGE(OTB)  
IF(OTB_FOUND)  
  INCLUDE(${OTB_USE_FILE})  
ELSE(OTB_FOUND)  
  MESSAGE(FATAL_ERROR  
      "Cannot build OTB project without OTB.  Please set OTB_DIR.")  
ENDIF(OTB_FOUND)

include(${QT_USE_FILE})

set(image_view_SRCS main.cpp MainWindow.cpp ImageWidget.cpp SaveSelectionDialog.cpp ChannelMappingDialog.cpp CBDialog.cpp AutoCBDialog.cpp HistogramDialog.cpp)

set(image_view_MOC_HDRS MainWindow.h ImageWidget.h SaveSelectionDialog.h ChannelMappingDialog.h CBDialog.h AutoCBDialog.h HistogramDialog.h Definitions.h)

set(data image_view.mod)

QT4_WRAP_CPP(image_view_MOC_SRCS ${image_view_MOC_HDRS})

add_executable(image_view ${image_view_SRCS} ${image_view_MOC_SRCS})

target_link_libraries(image_view ${QT_LIBRARIES} OTBCommon OTBIO)

install(TARGETS image_view DESTINATION bin)
install(FILES ${data} DESTINATION share/data/modules)
