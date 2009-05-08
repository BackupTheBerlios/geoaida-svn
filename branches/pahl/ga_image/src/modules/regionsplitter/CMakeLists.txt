INCLUDE_DIRECTORIES(.
        ${CMAKE_SOURCE_DIR}/pfm/libpfm
        ${PROJECT_SOURCE_DIR}/src/image
	)

SET(headers 
           garegionsplittert.h
           gasplit2regionst.h
)


SET(templateheaders
           garegionsplittert.hpp
           gasplit2regionst.hpp
)

INSTALL(FILES ${headers} DESTINATION include)
#INSTALL(FILES ${classheaders} DESTINATION include/Ga)
INSTALL(FILES ${templateheaders} DESTINATION include)

