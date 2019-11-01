cmake_minimum_required(VERSION 3.0.0)
project(CameraCalibration VERSION 0.1.0)

# 设置为 C++ 11版本
set(CMAKE_CXX_STANDARD 11)

include(CTest)
enable_testing()

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/3rdparty/opencv-3.4.7/share/OpenCV)

set(OpenCV_DIR /home/askeynil/devtools/libs/opencv-3.4.7/share/OpenCV)

find_package(OpenCV REQUIRED)
# 打印当前 OpenCV 版本和位置
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")


add_executable(CalibrationTool CalibrationTool.cc Calibrate.h Calibrate.cc)
target_link_libraries(
    CalibrationTool
    ${OpenCV_LIBS}
)

add_executable(DedistortTool DedistortTool.cc Calibrate.h Calibrate.cc)
target_link_libraries(
    DedistortTool
    ${OpenCV_LIBS}
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
