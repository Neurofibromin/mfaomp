#[[
mfaomp - Multiple Files At Once Media Player
Copyright (C) 2025  Neurofibromin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
]]

cmake_minimum_required(VERSION 3.28)
project(mfaomp
        VERSION 0.4.3
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
#set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/version.h"
        @ONLY
)

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


find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        WebEngineWidgets
        OPTIONAL_COMPONENTS Test
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)
pkg_check_modules(LIBVLCPP libvlcpp QUIET) # try to find libvlcpp system package

add_executable(mfaomp main.cpp
        AddVideo.cpp
        AddVideo.h
        MediaPlayers.h
        DropWidget.h
        MainWindow.cpp
        MainWindow.h
        SettingsDialog.cpp
        SettingsDialog.h
        MediaPlayers.cpp
        MediaPlayerImpls/VLCPlayerStruct.h
        MediaPlayerImpls/QMediaPlayerStruct.h
        MediaPlayerImpls/QWebEngineStruct.h
        MediaPlayerImpls/QWebEngineStruct.cpp)

target_include_directories(mfaomp PRIVATE "${CMAKE_CURRENT_BINARY_DIR}") #needed for version.h inclusion

# needed because Fedora disables build time fetching of dependencies, all sources must be downloaded beforehand
option(USE_PREDOWNLOADED_LIBVLCPP "Use predownloaded libvlcpp (disable FetchContent)" OFF)
# libvlcpp is available as a package for NixOS
option(USE_SYSTEM_PROVIDED_LIBVLCPP "Use libvlcpp system package (disable FetchContent, used on NixOS)" ${LIBVLCPP_FOUND})
option(USE_FETCHED_LIBVLCPP "Fetch libvlcpp (enable FetchContent, used by default)" ON)

set(LIBVLCPP_SOURCE_COUNT 0)
if (USE_PREDOWNLOADED_LIBVLCPP)
    math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
endif()
if (USE_SYSTEM_PROVIDED_LIBVLCPP)
    math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
endif()
if (USE_FETCHED_LIBVLCPP)
    math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
endif()

if (LIBVLCPP_SOURCE_COUNT GREATER 1)
    message(SEND_ERROR "Please select only one source for libvlcpp (USE_PREDOWNLOADED_LIBVLCPP, USE_SYSTEM_PROVIDED_LIBVLCPP, or USE_FETCHED_LIBVLCPP).")
elseif (LIBVLCPP_SOURCE_COUNT EQUAL 0)
    message(SEND_ERROR "No source for libvlcpp selected. Please enable one of USE_PREDOWNLOADED_LIBVLCPP, USE_SYSTEM_PROVIDED_LIBVLCPP, or USE_FETCHED_LIBVLCPP.")
endif()

if (USE_PREDOWNLOADED_LIBVLCPP)
    message(STATUS "Using predownloaded libvlcpp, FetchContent disabled")
    target_include_directories(mfaomp PRIVATE
            ${LIBVLC_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libvlcpp/vlcpp
    )
elseif (USE_SYSTEM_PROVIDED_LIBVLCPP)
    message(STATUS "Using system-provided libvlcpp, FetchContent disabled")
    target_include_directories(mfaomp PRIVATE
            ${LIBVLC_INCLUDE_DIRS}
            ${LIBVLCPP_INCLUDE_DIRS}/vlcpp
    )
elseif (USE_FETCHED_LIBVLCPP)
    message(STATUS "Fetching libvlcpp with FetchContent")
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
else ()
    message(SEND_ERROR "No source of libvlcpp provided!" )
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

if (Qt6Test_FOUND)
    get_target_property(_qt_test_exists Qt6::Test IMPORTED)
    message(STATUS "Qt6::Test target exists: ${_qt_test_exists}")
    message(STATUS "Qt6 Test found, enabling 'tests' subdirectory.")
    add_subdirectory(tests)
else ()
    message(STATUS "Qt6 Test not found, skipping 'tests' subdirectory.")
endif ()

include(CPack)
set(CPACK_PACKAGE_VENDOR "${PROJECT_COPYRIGHT_HOLDER}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHORS}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_LICENSE_FILE}")
set(CPACK_GENERATOR "NSIS")
