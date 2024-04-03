# cmake_minimum_required(VERSION 3.0)
# project(pravinnalla VERSION 1.0.0 LANGUAGES CXX)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)



#find_package(Qt6 COMPONENTS Sql REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Sql)



find_library(LIBHARU_LIBRARY hpdf )
if( ${LIBHARU_LIBRARY} STREQUAL "LIBHARU_LIBRARY-NOTFOUND")
    message("Please set LIBHARU_LIBRARY variable to location of libhpdf")
endif()

add_subdirectory(app)

