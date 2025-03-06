cmake_minimum_required(VERSION 3.5)
project(Adept LANGUAGES C CXX)

set(APPNAME ${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(SRC_FILES
	src/main.cpp
	src/FileReader.cpp
	src/FileReader.h
	src/Objects.h
	src/Grouper.h 
	src/Grouper.cpp
	src/FIleWriter.h
	src/FIleWriter.cpp
	src/BDController.h
	src/BDController.cpp
)

add_executable(Adept ${SRC_FILES})

include("${CMAKE_CURRENT_LIST_DIR}/cmake/FindFirebird.cmake")

find_package(Firebird 3.0 REQUIRED)

target_link_libraries(Adept
	PUBLIC Firebird::fbclient
)

add_custom_command(
        TARGET Adept POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/test.txt
                ${CMAKE_CURRENT_BINARY_DIR}/test.txt
)

