set(pro_SOURCE
	Device.h
	Device.cpp
	Cnw8302.h
	Cnw8302.cpp
	ControlCenter.cpp
	ControlCenter.h
	../version.h
	main.cpp
)
	
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/udplib)
include_directories(${CMAKE_SOURCE_DIR}/cli)
include_directories(${CMAKE_SOURCE_DIR})

include_directories($ENV{BOOSTLIB_DIR})
include_directories($ENV{DBXML_DIR}/include)

link_directories($ENV{BOOSTLIB_DIR}/stage/lib)
link_directories($ENV{DBXML_DIR}/lib)

ADD_EXECUTABLE(daemonconf
    ${pro_SOURCE}
    )

target_link_libraries(daemonconf
    udplib
    cli
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    )
    
if (UNIX)
	target_link_libraries(daemonconf   
		boost_filesystem-mt.a
		boost_system-mt.a
		boost_thread-mt.a
		boost_program_options-mt.a
		boost_date_time-mt.a
		boost_regex-mt.a
		rt
		)
endif (UNIX)