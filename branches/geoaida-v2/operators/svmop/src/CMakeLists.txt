include(FindPkgConfig)

find_package(OTB)
if(OTB_FOUND)
  include(${OTB_USE_FILE})
else(OTB_FOUND)
  message(FATAL_ERROR
      "Cannot build OTB project without OTB.  Please set OTB_DIR.")
endif(OTB_FOUND)

# pkg_check_modules(VIPS REQUIRED vipsCC-7.16)
# pkg_check_modules(OPENCV REQUIRED opencv)

# add_definitions(${VIPS_CFLAGS})
# add_definitions(${OPENCV_CFLAGS})
# include_directories(${VIPS_INCLUDE_DIRS})
# include_directories(${OPENCV_INCLUDE_DIRS})

set(sources
    feature_extractor.cpp
    log.cpp
    svm_classifier.cpp
    timer.cpp
)
set(class_sources svm_class.cpp)
set(train_sources svm_train.cpp)

set(headers
)

add_executable(svm_class ${sources} ${class_sources})
add_executable(svm_train ${sources} ${train_sources})

target_link_libraries(svm_class
    tiff
    OTBCommon
    OTBIO
    OTBLearning
)
target_link_libraries(svm_train
    tiff
    OTBCommon
    OTBIO
    OTBLearning
)

install(TARGETS svm_class svm_train RUNTIME DESTINATION bin)
install(PROGRAMS ga_td_svmop_multi DESTINATION bin)
install(FILES ga_svmop_multi.op DESTINATION share/data/operators)


