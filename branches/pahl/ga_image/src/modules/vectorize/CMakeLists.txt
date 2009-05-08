INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${PROJECT_SOURCE_DIR}/src/image
	)


SET(headers 
           gavectorizet.h
)

SET(templateheaders
           gavectorizet.hpp
)

install(FILES ${headers} DESTINATION include)
install(FILES ${templateheaders} DESTINATION include)
