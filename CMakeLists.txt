cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(VTKWave)

#Include QT
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(OpenMP REQUIRED)

#include VTK
find_package(VTK COMPONENTS)


#Add headers directory
include_directories(headers)



#Add source files
file(GLOB HEADER_FILES headers/*.h)
file(GLOB SOURCE_FILES src/*.cpp)

#qt_wrap_cpp(MOC_SOURCES mainwindow.h)

#Adding executable
add_executable(VTKWave ${SOURCE_FILES} ${HEADER_FILES} ${MOC_SOURCES})

source_group("Headers" FILES ${HEADER_FILES})
source_group("Sources" FILES ${SOURCE_FILES})

#Libraries linking (QT and VTK)
target_link_libraries(VTKWave PRIVATE ${VTK_LIBRARIES})
target_link_libraries(VTKWave PRIVATE ${QT_LIBRARIES})
target_link_libraries(VTKWave PRIVATE OpenMP::OpenMP_CXX)
add_custom_command(
    TARGET VTKWave PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/images
            ${CMAKE_CURRENT_BINARY_DIR}/images
)



if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTKWave: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
set_target_properties(VTKWave PROPERTIES AUTOMOC TRUE)
# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS VTKWave
  MODULES ${VTK_LIBRARIES}
)