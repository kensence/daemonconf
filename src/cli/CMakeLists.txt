set(cli_SOURCES
	clisession.cpp
	clisession.h
	json.h
	xml.h
    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/udplib)
include_directories(${CMAKE_SOURCE_DIR}/daemonconf)
include_directories($ENV{BOOSTLIB_DIR})
include_directories(${CMAKE_SOURCE_DIR})

link_directories($ENV{BOOSTLIB_DIR}/stage/lib)

ADD_LIBRARY(cli STATIC
    ${cli_SOURCES}
    )
#ADD_LIBRARY(cli_SHARED SHARED
#    ${cli_SOURCES}
#    )

#set_target_properties(cli_SHARED
#    PROPERTIES OUTPUT_NAME "cli")


#if (WIN32)
#	TARGET_LINK_LIBRARIES(cli_SHARED ws2_32)
#    SET_TARGET_PROPERTIES(cli PROPERTIES SUFFIX "_a.lib")
#endif (WIN32)

# Set output directory
set_target_properties(cli 
#cli_SHARED
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

