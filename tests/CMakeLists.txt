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

add_executable(mfaomp_tests tst_mfaomp.cpp)

target_include_directories(mfaomp_tests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(mfaomp_tests
        Qt6::Test
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        Qt::MultimediaWidgets
        Qt::WebEngineWidgets
#        mfaomp # Target "mfaomp" of type EXECUTABLE may not be linked into another target.
        #  One may link only to INTERFACE, OBJECT, STATIC or SHARED libraries, or to
        #  executables with the ENABLE_EXPORTS property set.
)

add_test(NAME mfaomp_test COMMAND mfaomp_tests)
