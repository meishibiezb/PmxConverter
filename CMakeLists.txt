cmake_minimum_required(VERSION 3.10)
project(learnOpenGL)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(PmxConverter
    PmxConverter.cpp  
)
add_executable(ModelReader
    ModelReader.cpp  
)

target_link_directories(PmxConverter PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)
target_link_libraries(PmxConverter
    assimp.dll
)

target_link_directories(ModelReader PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)
target_link_libraries(ModelReader
    assimp.dll
)