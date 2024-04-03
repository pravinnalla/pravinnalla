# cmake_minimum_required(VERSION 3.0)
# project(pravinnalla VERSION 1.0.0 LANGUAGES CXX)

add_executable(pravinnalla
  main.cpp
  mydatabase.h mydatabase.cpp
  csvfilehandler.h csvfilehandler.cpp
  pdfgenerator.h pdfgenerator.cpp
)

target_link_libraries(pravinnalla PRIVATE Qt5::Core Qt5::Sql hpdf)

INSTALL(TARGETS pravinnalla DESTINATION bin)
INSTALL(TARGETS pravinnalla
        LIBRARY
        DESTINATION lib)
