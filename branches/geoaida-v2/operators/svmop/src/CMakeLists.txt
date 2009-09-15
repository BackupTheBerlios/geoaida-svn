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
	main.cpp
	svm_classifier.cpp
	timer.cpp
)

set(headers
)

add_executable(svmop_4-2 ${sources})

target_link_libraries(svmop_4-2
	tiff
	OTBCommon
	OTBIO
	OTBLearning
)

install(TARGETS svmop_4-2 RUNTIME DESTINATION bin)

# install(TARGETS ga_gisop_multi DESTINATION bin)

# install(FILES ga_gisop_multi.op
#   DESTINATION share/data/operators)
# 
# install(PROGRAMS ga_td_gisop_multi DESTINATION bin)
