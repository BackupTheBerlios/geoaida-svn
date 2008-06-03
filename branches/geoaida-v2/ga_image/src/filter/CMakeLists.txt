INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${PROJECT_SOURCE_DIR}/src/image
	)

SET(headers 
	   gaalgo.h
           gadowindowt.h
           gagaussian.h
           gagradient.h
           gagradientt.h
           gahough.h
           gakirsch.h
           gavariance.h
)

SET(sources 
	   gacalchistogram.cpp
           gacanny.cpp
           gagaussian.cpp
           gagradient.cpp
           gahough.cpp
           gatohsv.cpp
           gatoluminance.cpp
           gatondvi.cpp
           gavariance.cpp
)

add_library(gafilter STATIC ${sources})
install(TARGETS gafilter DESTINATION lib)
install(FILES ${headers} DESTINATION include)
#install(FILES ${classheaders} DESTINATION include/Ga)

#add_executable(test_filter test_image.cpp)
#target_link_libraries(test_filter gaimage pfm netpbm)


