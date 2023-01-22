cmake_minimum_required (VERSION 3.20)

include(FetchContent)
include(CheckLanguage)

project (graphs LANGUAGES CXX C)


include(GNUInstallDirs)

set( CMAKE_CXX_FLAGS "-std=c++20" )
add_compile_options( -g )
add_compile_options( -Wfatal-errors )

find_package( OpenSSL QUIET )
find_package( OpenGL REQUIRED COMPONENTS OpenGL )
find_package( GLUT REQUIRED )

set( SOURCES src/maze.cpp src/main.cpp src/player.cpp src/opengldisplay.cpp src/openglsetup.cpp )

add_executable( maze ${SOURCES} )
target_link_libraries( maze PRIVATE ${OPENGL_LIBRARIES} ${GLUT_glut_LIBRARY} ) 
# set_target_properties( maze PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ".." )

# add_executable( bfs src/bfs.cpp )
