enable_testing()
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

set(target_sources ${MFAOMP_SOURCES})
list(TRANSFORM target_sources PREPEND "../")
add_executable(mfaomp_ctests
        test_main.cpp
        ${target_sources}
)

target_include_directories(mfaomp_ctests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${LIBVLC_INCLUDE_DIRS}
        ${libvlcpp_SOURCE_DIR}/vlcpp # this is very unportable, assumes fetchcontent in root cmakelists
)

target_link_libraries(mfaomp_ctests PRIVATE
        ${MFAOMP_LIBRARIES}
        Qt::Test
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME mfaomp_ctests COMMAND mfaomp_ctests)