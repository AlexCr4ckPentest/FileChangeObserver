cmake_minimum_required(VERSION 3.1)

project(file-change-observer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Os -Wall -Wextra")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)

if(Boost_FOUND)
    file(GLOB SRC_PATH src/*.cpp)
    file(GLOB INC_PATH include/*.h)
    add_executable(${PROJECT_NAME} ${SRC_PATH} ${INC_PATH})
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} Boost::filesystem Boost::program_options ws2_32)
    elseif(UNIX)
        target_link_libraries(${PROJECT_NAME} Boost::filesystem Boost::program_options pthread)
    endif()
endif()
