cmake_minimum_required(VERSION 3.28)
project(mfaomp
        VERSION 0.4.0
        DESCRIPTION "Multiple Files At Once Media Player"
        HOMEPAGE_URL "https://github.com/Neurofibromin/mfaomp"
)
set(PROJECT_AUTHORS "Neurofibromin <125222560+Neurofibromin@users.noreply.github.com>")
set(PROJECT_COPYRIGHT_YEAR "2025")
set(PROJECT_COPYRIGHT_HOLDER "Neurofibromin")
set(PROJECT_LICENSE_NAME "GPLv3")
set(PROJECT_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
#        WebEngine
        WebEngineWidgets
        REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)

add_executable(mfaomp main.cpp
        AddVideo.cpp
        AddVideo.h
        MediaPlayers.h)

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
#        Qt::WebEngine
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
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_LICENSE_FILE}") # CPack will display this during installation
