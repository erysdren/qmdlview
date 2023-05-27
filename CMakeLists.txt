
# project
cmake_minimum_required(VERSION 3.3)
project(qmdlview LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
add_subdirectory(${PROJECT_SOURCE_DIR}/source/thirdparty/glui)

# sources
set(QMDLVIEW_SOURCES
	${PROJECT_SOURCE_DIR}/source/mdl.c
	${PROJECT_SOURCE_DIR}/source/thirdparty/tinyfiledialogs.c
	${PROJECT_SOURCE_DIR}/source/main.cpp
)

add_executable(qmdlview ${QMDLVIEW_SOURCES})
target_link_libraries(qmdlview PUBLIC GLUT::GLUT)
target_link_libraries(qmdlview PUBLIC OpenGL::GL)
target_link_libraries(qmdlview PUBLIC OpenGL::GLU)
target_link_libraries(qmdlview PUBLIC glui_static)
target_include_directories(qmdlview PUBLIC ${PROJECT_SOURCE_DIR}/source/thirdparty/glui/include)
target_include_directories(qmdlview PUBLIC ${PROJECT_SOURCE_DIR}/source/thirdparty)
