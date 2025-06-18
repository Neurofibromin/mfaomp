# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(mfaomp_tests)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)
find_package(Qt6 REQUIRED COMPONENTS
             Core
             Gui
             Widgets
             Multimedia
             MultimediaWidgets
             WebEngineWidgets
             Test
)


set(target_sources ${MFAOMP_SOURCES})
list(TRANSFORM target_sources PREPEND "../")
add_executable(mfaomp_tests
               SettingTests.cpp
               ${target_sources}
               AddVideoTests.cpp
)

target_include_directories(mfaomp_tests PRIVATE
                           ${CMAKE_SOURCE_DIR}
                           ${CMAKE_BINARY_DIR}
                           "${CMAKE_SOURCE_DIR}/MediaPlayerImpls"
                           ${LIBVLC_INCLUDE_DIRS}
                           ${libvlcpp_SOURCE_DIR}/vlcpp # this is very unportable, assumes fetchcontent in root cmakelists
)

target_link_libraries(mfaomp_tests
                      Qt6::Test
                      ${MFAOMP_LIBRARIES}
                      Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME mfaomp_test COMMAND mfaomp_tests)
