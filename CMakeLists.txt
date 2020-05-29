cmake_minimum_required(VERSION 3.00)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

set(MOD_LIST API BufferManager CatalogManager IndexManager Interpreter RecordManager)

foreach(modName IN LISTS MOD_LIST)
    include_directories(./${modName}/)
    add_library(${modName} STATIC ./${modName}/${modName}.cpp)
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
add_executable(MiniSQL ./MiniSQL.cpp)

foreach(modName IN LISTS MOD_LIST)
    target_link_libraries(MiniSQL ${modName})
endforeach()