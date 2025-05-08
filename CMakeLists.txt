cmake_minimum_required(VERSION 3.25)
project(obj-renderer LANGUAGES CXX)

# Project wide setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

find_package(SDL2 REQUIRED)

add_executable(renderer
	src/main.cpp
	src/model.cpp
	src/renderer.cpp
	src/vector.cpp
	src/threadpool.cpp
)

target_include_directories(renderer
	PRIVATE
		${SDL2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(renderer ${SDL2_LIBRARIES})
