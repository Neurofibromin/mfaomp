cmake_minimum_required(VERSION 3.28)
project(mfaomp
        VERSION 0.4.1
        DESCRIPTION "Multiple Files At Once Media Player"
        HOMEPAGE_URL "https://github.com/Neurofibromin/mfaomp"
)
set(PROJECT_AUTHORS "Neurofibromin <125222560+Neurofibromin@users.noreply.github.com>")
set(PROJECT_COPYRIGHT_YEAR "2025")
set(PROJECT_COPYRIGHT_HOLDER "Neurofibromin")
set(PROJECT_LICENSE_NAME "GPL-3.0-or-later")
set(PROJECT_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# VCPKG
# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake
if (WIN32)
    option(USE_VCPKG "Enable vcpkg for qt (not system libraries)" ON)
    if (NOT DEFINED VCPKG_ROOT)
        set(VCPKG_ROOT "C:/vcpkg")
    endif ()
else ()
    option(USE_VCPKG "Enable vcpkg for qt (not system libraries)" OFF)
endif()
if (USE_VCPKG)
    message(STATUS "Enable vcpkg for Qt")
    if (DEFINED CMAKE_TOOLCHAIN_FILE)
    elseif (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    else()
        set(VCPKG_ROOT "$ENV{HOME}/.vcpkg")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    endif()
else()
    message(STATUS "Using system provided Qt")
endif()


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        WebEngineWidgets
        REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)

add_executable(mfaomp main.cpp
        AddVideo.cpp
        AddVideo.h
        MediaPlayers.h)

# needed because Fedora disables build time fetching of dependencies, all sources must be downloaded beforehand
option(USE_PREDOWNLOADED_LIBVLCPP "Use predownloaded libvlcpp (disable FetchContent)" OFF)
if (USE_PREDOWNLOADED_LIBVLCPP)
    message(STATUS "Using system-provided libvlcpp, FetchContent disabled")
    target_include_directories(mfaomp PRIVATE
            ${LIBVLC_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libvlcpp/vlcpp
    )
else()
    include(FetchContent)
    FetchContent_Declare(
            libvlcpp #libvlcpp has no cmakelists, but it's header only so only including from the fetched repo
            GIT_REPOSITORY https://github.com/videolan/libvlcpp.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(libvlcpp)
    target_include_directories(mfaomp PRIVATE
            ${LIBVLC_INCLUDE_DIRS}
            ${libvlcpp_SOURCE_DIR}/vlcpp
    )
endif()

target_link_libraries(mfaomp
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        Qt::MultimediaWidgets
        Qt::WebEngineWidgets
        ${LIBVLC_LIBRARIES}
)

install(TARGETS mfaomp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR} # For macOS ?
)

install(FILES "${PROJECT_LICENSE_FILE}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}"
        RENAME LICENSE.txt
)

install(FILES mfaomp.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

include(CPack)
set(CPACK_PACKAGE_VENDOR "${PROJECT_COPYRIGHT_HOLDER}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHORS}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_LICENSE_FILE}")
set(CPACK_GENERATOR "NSIS")