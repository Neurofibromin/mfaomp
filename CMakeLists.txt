#root CMakeLists.txt
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
        VERSION 0.6.5
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

option(ENABLE_QTMULTIMEDIA "Enable Qt Multimedia backend" ON)
option(ENABLE_LIBVLC "Enable VLC backend" ON)
option(ENABLE_QTWEBENGINE "Enable Qt WebEngine backend" ON)
option(ENABLE_CATCH "Enable Catch testing" ON)
option(USE_VCPKG "Enable vcpkg for qt (not system libraries)" OFF)
option(USE_PREDOWNLOADED_LIBVLCPP "Use predownloaded libvlcpp (disable FetchContent)" OFF) # needed because Fedora disables build time fetching of dependencies, all sources must be downloaded beforehand
option(USE_SYSTEM_PROVIDED_LIBVLCPP "Use libvlcpp system package (disable FetchContent, used on NixOS)" OFF) # libvlcpp is available as a package for NixOS
option(USE_FETCHED_LIBVLCPP "Fetch libvlcpp (enable FetchContent, used by default)" ON)
set(HAVE_QTMULTIMEDIA FALSE)
set(HAVE_LIBVLC FALSE)
set(HAVE_QTWEBENGINE FALSE)

# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake
if (WIN32 AND USE_VCPKG AND NOT DEFINED VCPKG_ROOT AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_ROOT "C:/vcpkg")
endif ()
if (USE_VCPKG)
    message(STATUS "Enabling vcpkg")
    if (DEFINED CMAKE_TOOLCHAIN_FILE)
    elseif (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    elseif (DEFINED VCPKG_ROOT)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    else ()
        set(VCPKG_ROOT "$ENV{HOME}/.vcpkg")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    endif ()
endif ()
#set(CMAKE_FIND_DEBUG_MODE 1)
find_package(Qt6 REQUIRED COMPONENTS
             Core
             Gui
             Widgets
             OPTIONAL_COMPONENTS
             Multimedia
             MultimediaWidgets
             WebEngineWidgets
             Test
)
#set(CMAKE_FIND_DEBUG_MODE 0)
# VLC
if (ENABLE_LIBVLC AND NOT WIN32) #TODO: solve libvlc for windows without pkgconfig
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBVLC libvlc)
    if (LIBVLC_FOUND)
        message(STATUS "libVLC backend enabled")
        set(HAVE_LIBVLC TRUE)
    else ()
        message(WARNING "libVLC not found; disabling backend")
        set(HAVE_LIBVLC FALSE)
    endif ()
    if (USE_SYSTEM_PROVIDED_LIBVLCPP)
        pkg_check_modules(LIBVLCPP libvlcpp QUIET)
        if (LIBVLCPP_FOUND)
            message(STATUS "libvlcpp system package found")
        else ()
            message(FATAL_ERROR "System provided libvlcpp package not found")
        endif ()
    endif ()
elseif ()
    message(WARNING "libVLC not supported on windows; disabling backend")
    set(HAVE_LIBVLC FALSE)
endif ()

# Qt Multimedia
if (ENABLE_QTMULTIMEDIA)
    if (Qt6Multimedia_FOUND AND Qt6MultimediaWidgets_FOUND)
        message(STATUS "QtMultimedia backend enabled")
        set(HAVE_QTMULTIMEDIA TRUE)
    else ()
        message(WARNING "QtMultimedia not found; disabling backend")
        set(HAVE_QTMULTIMEDIA FALSE)
    endif ()
endif ()

# WebEngine
if (ENABLE_QTWEBENGINE)
    if (Qt6WebEngineWidgets_FOUND)
        message(STATUS "QtWebEngine backend enabled")
        set(HAVE_QTWEBENGINE TRUE)
    else ()
        message(WARNING "QtWebEngineWidgets not found; disabling backend")
        set(HAVE_QTWEBENGINE FALSE)
    endif ()
endif ()

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()
find_package(Boost 1.80 COMPONENTS system filesystem REQUIRED)
if (Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else ()
    message(FATAL_ERROR "Boost not found. Please install Boost development libraries.")
endif ()

set(MFAOMP_SOURCES
    AddVideo.cpp
    AddVideo.h
    BackendAvailability.cpp
    BackendAvailability.h
    DropWidget.h
    MainWindow.cpp
    MainWindow.h
    MediaPlayers.h
    SettingsDialog.cpp
    SettingsDialog.h
    MediaPlayerImpls/PlayerFactory.cpp
    MediaPlayerImpls/PlayerFactory.h
    MediaPlayerImpls/CurrentBackEndStatusSingleton.h
)

if (HAVE_LIBVLC)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/VLCPlayerStruct.h)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/VLCPlayerStruct.cpp)
endif ()
if (HAVE_QTMULTIMEDIA)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/QMediaPlayerStruct.h)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/QMediaPlayerStruct.cpp)
endif ()
if (HAVE_QTWEBENGINE)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/QWebEngineStruct.h)
    list(APPEND MFAOMP_SOURCES MediaPlayerImpls/QWebEngineStruct.cpp)
endif ()

add_executable(mfaomp ${MFAOMP_SOURCES} main.cpp)

target_include_directories(mfaomp PRIVATE
                           "${CMAKE_CURRENT_BINARY_DIR}" #needed for version.h inclusion
                           "${CMAKE_CURRENT_SOURCE_DIR}"
                           "MediaPlayerImpls"
                           ${Boost_INCLUDE_DIRS}
)
if (HAVE_LIBVLC)
    set(LIBVLCPP_SOURCE_COUNT 0)
    if (USE_PREDOWNLOADED_LIBVLCPP)
        math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
    endif ()
    if (USE_SYSTEM_PROVIDED_LIBVLCPP)
        math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
    endif ()
    if (USE_FETCHED_LIBVLCPP)
        math(EXPR LIBVLCPP_SOURCE_COUNT "${LIBVLCPP_SOURCE_COUNT} + 1")
    endif ()

    if (LIBVLCPP_SOURCE_COUNT GREATER 1)
        message(SEND_ERROR "Please select only one source for libvlcpp (USE_PREDOWNLOADED_LIBVLCPP, USE_SYSTEM_PROVIDED_LIBVLCPP, or USE_FETCHED_LIBVLCPP).")
    elseif (LIBVLCPP_SOURCE_COUNT EQUAL 0)
        message(SEND_ERROR "No source for libvlcpp selected. Please enable one of USE_PREDOWNLOADED_LIBVLCPP, USE_SYSTEM_PROVIDED_LIBVLCPP, or USE_FETCHED_LIBVLCPP.")
    endif ()

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
        message(SEND_ERROR "No source of libvlcpp provided!")
    endif ()
endif ()
set(MFAOMP_LIBRARIES
    Qt::Core
    Qt::Gui
    Qt::Widgets
    ${Boost_LIBRARIES}
)

# Conditionally add backend-specific Qt libraries and VLC library
if (HAVE_QTMULTIMEDIA)
    list(APPEND MFAOMP_LIBRARIES Qt::Multimedia Qt::MultimediaWidgets)
endif ()
if (HAVE_QTWEBENGINE)
    list(APPEND MFAOMP_LIBRARIES Qt::WebEngineWidgets)
endif ()
if (HAVE_LIBVLC)
    list(APPEND MFAOMP_LIBRARIES ${LIBVLC_LIBRARIES})
endif ()

target_link_libraries(mfaomp ${MFAOMP_LIBRARIES})

install(TARGETS mfaomp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR} # For macOS ?
)

install(FILES "${PROJECT_LICENSE_FILE}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}"
        RENAME LICENSE.txt
)

install(FILES mfaomp.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

if (Qt6Test_FOUND AND HAVE_QTMULTIMEDIA AND HAVE_QTWEBENGINE AND HAVE_LIBVLC AND ENABLE_CATCH)
    get_target_property(_qt_test_exists Qt6::Test IMPORTED)
    message(STATUS "Qt6::Test target exists: ${_qt_test_exists}")
    message(STATUS "Qt6 Test found, enabling 'tests' subdirectory.")
    add_subdirectory(tests)
else ()
    message(STATUS "Qt6 Test not found or backends missing, skipping 'tests' subdirectory.")
endif ()

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
        @ONLY
)

include(CPack)
set(CPACK_PACKAGE_VENDOR "${PROJECT_COPYRIGHT_HOLDER}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHORS}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_LICENSE_FILE}")
set(CPACK_GENERATOR "NSIS")
