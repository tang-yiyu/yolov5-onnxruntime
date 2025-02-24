cmake_minimum_required(VERSION 3.0.0)
project(yolo_ort)

set(ONNXRUNTIME_DIR "D:/lib/onnxruntime")
message(STATUS "ONNXRUNTIME_DIR: ${ONNXRUNTIME_DIR}")

find_package(OpenCV REQUIRED PATHS "D:/lib/opencv/build" NO_DEFAULT_PATH)
# find_package(OpenCV REQUIRED)


include_directories(
    "include/"
    ${OpenCV_INCLUDE_DIRS}
    # ${ONNXRUNTIME_DIR}/include
)

add_executable(yolo_ort
               src/main.cpp
               src/detector.cpp
               src/utils.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(yolo_ort PRIVATE "${ONNXRUNTIME_DIR}/include")
# link_directories("${ONNXRUNTIME_DIR}/lib")
target_compile_features(yolo_ort PRIVATE cxx_std_14)
target_link_libraries(yolo_ort ${OpenCV_LIBS})

if (WIN32)
    target_link_libraries(yolo_ort "${ONNXRUNTIME_DIR}/lib/onnxruntime.lib")
endif(WIN32)

if (UNIX)
    target_link_libraries(yolo_ort "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
endif(UNIX)

