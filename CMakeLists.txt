cmake_minimum_required(VERSION 3.19)
project(objects_tracking LANGUAGES CXX)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Test)
find_package(OpenCV REQUIRED )
find_package(fmt CONFIG REQUIRED)
find_package(log4cplus CONFIG REQUIRED)
find_package(cpptrace REQUIRED)
file(COPY res DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
qt_standard_project_setup()

qt_add_executable(objects_tracking
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/video/videothread.cpp src/video/videothread.h src/widgets/main_window/mainwindow.cpp src/widgets/main_window/mainwindow.h src/main.cpp
    src/video/object_detector/ObjectDetector.h src/video/object_detector/ObjectDetector.cpp

    src/video/object_detector/Detection.h
    src/logger/logger.h src/logger/logger.cpp
    src/core/exception/abstract_exception.h
    src/core/exception/listener_already_exists_exception.h
    src/application.h
    src/core/qthread/base_thread.h
    src/core/qthread/q_thread_exception.h
    src/core/qthread/base_thread.cpp

    src/core/area/area.h
    src/core/area/area.cpp

    src/frame_processors/base_frame_processor/base_frame_processor.h
    src/frame_processors/base_frame_processor/base_frame_processor.cpp
    src/frame_processors/base_frame_processor/frame_processor_listener.h

    src/frame_processors/optical_flow_frame_processor/optical_flow_frame_processor.h 
    src/frame_processors/optical_flow_frame_processor/optical_flow_frame_processor.cpp

    src/frame_processors/moved_areas_frame_processor/moved_areas_frame_processor.h 
    src/frame_processors/moved_areas_frame_processor/moved_areas_frame_processor.cpp



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
        Qt::Test
        ${OpenCV_LIBS}
        fmt::fmt
        cpptrace::cpptrace
        log4cplus::log4cplus
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

# Тесты (включаем поддержку CTest)
enable_testing()
add_subdirectory(test)
