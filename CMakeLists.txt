cmake_minimum_required(VERSION 3.19)
project(objects_tracking LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED )
file(COPY res DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
qt_standard_project_setup()

qt_add_executable(objects_tracking
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/video/videothread.cpp src/video/videothread.h src/widgets/main_window/mainwindow.cpp src/widgets/main_window/mainwindow.h src/main.cpp
    src/video/optical_flow/OpticalFlow.h src/video/optical_flow/OpticalFlow.cpp
    src/video/object_detector/ObjectDetector.h src/video/object_detector/ObjectDetector.cpp

    src/video/object_detector/Detection.h

)

# Define the source and destination directories
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
# Use CMAKE_CURRENT_BINARY_DIR to place the copy in the corresponding build directory
set(DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/res")

# Add a custom target to perform the copy operation
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SOURCE_DIR}"
    "${DESTINATION_DIR}"
    COMMENT "Copying assets to build directory"
)

add_dependencies(objects_tracking copy_assets)

target_link_libraries(objects_tracking
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${OpenCV_LIBS}
)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories("./src")
include(GNUInstallDirs)

install(TARGETS objects_tracking
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET objects_tracking
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
