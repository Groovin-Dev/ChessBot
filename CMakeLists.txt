cmake_minimum_required(VERSION 3.25)
project(ChessBot)

# Include Qt5 package
find_package(Qt5 COMPONENTS Core Gui Widgets Svg REQUIRED)

# Include X11 package
find_package(X11 REQUIRED)

# Set your sources
set(SOURCES main.cpp x11wrapper.cpp x11wrapper.h)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Consolidate target_link_libraries calls into one
target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Svg
        ${X11_LIBRARIES}
        )

# Include X11
include_directories(${X11_INCLUDE_DIR})
