# the name of the project
project(daemonconf)
 
cmake_minimum_required(VERSION 2.6.0)

# find and setup threads library for the project
find_package(Threads)

# Set verbose output to make
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Set defines
#ADD_DEFINITIONS(-DGNU -D_GNU_SOURCE -D_REENTRANT)

if (WIN32)
	ADD_DEFINITIONS("-FR")
	#SET(CMAKE_CXX_FLAGS "/EHsc /Fd$(IntDir)\\$(ProjectName).pdb")
endif (WIN32)

#SET(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL:NO")
#SET(CMAKE_SHARED_LINKER_FLAGS "-EL")
#SET(CMAKE_MODULE_LINKER_FLAGS "-EL")

# tell cmake to process CMakeLists.txt in the subdirectories
add_subdirectory(daemonconf)
add_subdirectory(udplib)
add_subdirectory(cli)

