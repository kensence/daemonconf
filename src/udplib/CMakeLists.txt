set(udplib_SOURCES
	OrderBuffer.cpp
	RawPacket.cpp
	UdpSocket.cpp
	IoServer.h
	OrderBuffer.h
	RawPacket.h
	UdpSocket.h
    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories($ENV{BOOSTLIB_DIR})

link_directories($ENV{BOOSTLIB_DIR}/stage/lib)

ADD_LIBRARY(udplib STATIC
    ${udplib_SOURCES}
    )
#ADD_LIBRARY(udplib_SHARED SHARED
#    ${udplib_SOURCES}
#    )

#set_target_properties(udplib_SHARED
#    PROPERTIES OUTPUT_NAME "udplib")


#if (WIN32)
#	TARGET_LINK_LIBRARIES(udplib_SHARED ws2_32)
#    SET_TARGET_PROPERTIES(udplib PROPERTIES SUFFIX "_a.lib")
#endif (WIN32)

# Set output directory
set_target_properties(udplib 
#udplib_SHARED
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

