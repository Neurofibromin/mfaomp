# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(mfaomp_tests)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        WebEngineWidgets
        Test
)
enable_testing(true)
message(STATUS "source dir: " ${CMAKE_SOURCE_DIR})
set(target_sources ${MFAOMP_SOURCES})
list(TRANSFORM target_sources PREPEND "../")

add_executable(mfaomp_qtests
        tst_mfaomp.cpp
        ${target_sources}
        )


target_include_directories(mfaomp_qtests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${LIBVLC_INCLUDE_DIRS}
        ${libvlcpp_SOURCE_DIR}/vlcpp # this is very unportable, assumes fetchcontent in root cmakelists
)

target_link_libraries(mfaomp_qtests
        Qt6::Test
        ${MFAOMP_LIBRARIES}
)

include(CTest)
add_test(NAME mfaomp_qtest COMMAND mfaomp_qtests)
