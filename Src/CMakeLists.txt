cmake_minimum_required(VERSION 3.00)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

set(MOD_LIST BufferManager CatalogManager IndexManager Interpreter RecordManager)

include_directories(./)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

foreach(modName IN LISTS MOD_LIST)
    include_directories(./${modName}/)
    add_library(${modName} STATIC ./${modName}/${modName}.cpp)
endforeach()

add_executable(MiniSQL ./MiniSQL.cpp)

foreach(modName IN LISTS MOD_LIST)
    target_link_libraries(MiniSQL optimized ${modName})
endforeach()
