cmake_minimum_required(VERSION 3.7)

project(LHCSim)

SET(CMAKE_CXX_FLAGS "-O2 -std=c++17")
SET(CMAKE_EXE_LINKER_FLAGS "-v")

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

message(STATUS "CMAKE source dir : ${CMAKE_SOURCE_DIR}")
#message(STATUS "executable : ${SOURCES}")
add_executable(LHCSim ${SOURCES})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)
find_package(Freetype REQUIRED)
find_package(Pythia8 REQUIRED)


message(STATUS "GLFW lib : ${GLFW_LIBRARY}")
#find_package(GLFW REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
message(STATUS "Cmake source dir : ${CMAKE_SOURCE_DIR}")
target_link_libraries(LHCSim ${GLEW_LIBRARY})
target_link_libraries(LHCSim ${GLUT_LIBRARY})
target_link_libraries(LHCSim ${OPENGL_LIBRARY})
target_link_libraries(LHCSim ${GLFW_LIBRARY})
target_link_libraries(LHCSim ${GLM_LIBRARY})
target_link_libraries(LHCSim ${FREETYPE_LIBRARY})
target_link_libraries(LHCSim ${PYTHIA8_LIBRARY})

#target_include_directories(LHCSim PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(LHCSim PUBLIC ${GLEW_INCLUDE_DIRS})
target_include_directories(LHCSim PUBLIC ${GLM_INCLUDE_DIRS})
target_include_directories(LHCSim PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(LHCSim PUBLIC ${PYTHIA8_INCLUDE_DIRS})

if (FREETYPE_FOUND)
	message(STATUS "freetype path: ${FREETYPE_INCLUDE_DIRS}")
	message(STATUS "freetype lib: ${FREETYPE_LIBRARY}")
endif(FREETYPE_FOUND)
if (PYTHIA8_FOUND)
	message(STATUS "Pythia8 path: ${PYTHIA8_INCLUDE_DIRS}")
	message(STATUS "Pythia8 lib:  ${PYTHIA8_LIBRARY}")
endif(PYTHIA8_FOUND)

