project(ReactNativeBlobJSIHelper)
cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "react-native-blob-jsi-helper")
set (BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(
        "../cpp"
)

add_library(reactnativeblobjsihelper  # <-- Library name
        SHARED
        src/main/cpp/cpp-adapter.cpp
        "../cpp/JSI Utils/TypedArray.cpp"
)

# Configure C++ 17
set_target_properties(
        reactnativeblobjsihelper PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)


find_package(ReactAndroid REQUIRED CONFIG)
find_library(log-lib log)

target_link_libraries(
        reactnativeblobjsihelper
        ${log-lib}          # <-- Logcat logger
        ReactAndroid::jsi   # <-- JSI
        android             # <-- Android JNI core
)
