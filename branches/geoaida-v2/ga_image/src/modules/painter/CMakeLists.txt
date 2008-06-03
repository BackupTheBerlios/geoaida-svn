include_directories(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${PROJECT_SOURCE_DIR}/src/image
	)

set(headers 
           gadraw.h
           gadrawt.h
	   gapainter.h
)

set(sources 
           gadraw.cpp 
	   gapainter.cpp
)
set(classheaders
          Painter
)

add_library(gapainter STATIC ${sources})
install(TARGETS gapainter DESTINATION lib)
install(FILES ${headers} DESTINATION include)
install(FILES ${classheaders} DESTINATION include/Ga)

add_executable(painttest test_painter.cpp)
target_link_libraries(painttest gaimage gapainter pfm netpbm)



